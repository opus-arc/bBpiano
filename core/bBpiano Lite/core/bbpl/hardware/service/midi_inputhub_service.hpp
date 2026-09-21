#ifndef BBPL_MIDI_INPUTHUB_SERVICE_HPP
#define BBPL_MIDI_INPUTHUB_SERVICE_HPP

#include <CoreMIDI/CoreMIDI.h>

#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

class MidiInputHub {
 public:
  using MidiMessage = std::span<const std::uint8_t>;
  using MessageHandler = std::function<void(MidiMessage)>;

 private:
  struct HandlerSlot {
    explicit HandlerSlot(MessageHandler new_handler)
        : handler(std::move(new_handler)) {}

    void invoke(MidiMessage message) noexcept {
      {
        std::lock_guard lock(mutex);
        if (!enabled) {
          return;
        }
        ++active_calls;
      }

      try {
        handler(message);
      } catch (...) {
        // Never let an exception cross the CoreMIDI C callback boundary.
      }

      {
        std::lock_guard lock(mutex);
        --active_calls;
      }
      condition.notify_all();
    }

    void disable_and_synchronize() noexcept {
      std::unique_lock lock(mutex);
      enabled = false;
      condition.wait(lock, [this] { return active_calls == 0; });
      handler = {};
    }

    bool is_enabled() const noexcept {
      std::lock_guard lock(mutex);
      return enabled;
    }

    mutable std::mutex mutex;
    std::condition_variable condition;
    MessageHandler handler;
    std::size_t active_calls = 0;
    bool enabled = true;
  };

 public:
  class Subscription {
   public:
    Subscription() noexcept = default;
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept
        : slot_(std::exchange(other.slot_, {})) {}

    Subscription& operator=(Subscription&& other) noexcept {
      if (this != &other) {
        reset();
        slot_ = std::exchange(other.slot_, {});
      }
      return *this;
    }

    ~Subscription() { reset(); }

    void reset() noexcept {
      if (slot_) {
        slot_->disable_and_synchronize();
        slot_.reset();
      }
    }

    explicit operator bool() const noexcept { return static_cast<bool>(slot_); }

   private:
    friend class MidiInputHub;
    explicit Subscription(std::shared_ptr<HandlerSlot> slot)
        : slot_(std::move(slot)) {}

    std::shared_ptr<HandlerSlot> slot_;
  };

  static constexpr std::uint8_t k_sustain_pedal_controller = 64;
  static constexpr std::uint8_t k_sostenuto_pedal_controller = 66;
  static constexpr std::uint8_t k_soft_pedal_controller = 67;
  static constexpr std::uint8_t k_harmonic_pedal_controller = 68;

  MidiInputHub() = default;
  MidiInputHub(const MidiInputHub&) = delete;
  MidiInputHub& operator=(const MidiInputHub&) = delete;
  MidiInputHub(MidiInputHub&&) = delete;
  MidiInputHub& operator=(MidiInputHub&&) = delete;

  ~MidiInputHub() { stop(); }

  Subscription subscribe(MessageHandler handler) {
    if (!handler) {
      throw std::invalid_argument("Invalid MIDI message handler.");
    }

    auto slot = std::make_shared<HandlerSlot>(std::move(handler));
    std::lock_guard lock(state_mutex_);
    erase_disabled_handlers_locked();
    handlers_.push_back(slot);
    return Subscription(std::move(slot));
  }

  void start() {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);
    {
      std::lock_guard state_lock(state_mutex_);
      if (running_) {
        return;
      }
    }

    const ItemCount source_count = MIDIGetNumberOfSources();
    if (source_count == 0) {
      throw std::runtime_error("No MIDI input device found.");
    }

    MIDIClientRef client = 0;
    MIDIPortRef input_port = 0;
    std::vector<std::unique_ptr<SourceConnection>> connections;

    OSStatus status = MIDIClientCreate(CFSTR("bbpl MIDI Input Hub"), nullptr,
                                       nullptr, &client);
    if (status != noErr || client == 0) {
      throw std::runtime_error("Failed to create CoreMIDI client.");
    }

    status = MIDIInputPortCreate(client, CFSTR("bbpl MIDI Input Port"),
                                 &MidiInputHub::midi_read_proc, nullptr,
                                 &input_port);
    if (status != noErr || input_port == 0) {
      MIDIClientDispose(client);
      throw std::runtime_error("Failed to create CoreMIDI input port.");
    }

    connections.reserve(static_cast<std::size_t>(source_count));
    for (ItemCount index = 0; index < source_count; ++index) {
      const MIDIEndpointRef source = MIDIGetSource(index);
      if (source == 0) {
        continue;
      }

      auto connection = std::make_unique<SourceConnection>();
      connection->hub = this;
      connection->source = source;
      status = MIDIPortConnectSource(input_port, source, connection.get());
      if (status == noErr) {
        connections.push_back(std::move(connection));
      }
    }

    if (connections.empty()) {
      MIDIPortDispose(input_port);
      MIDIClientDispose(client);
      throw std::runtime_error("No MIDI input device could be opened.");
    }

    {
      std::lock_guard state_lock(state_mutex_);
      client_ = client;
      input_port_ = input_port;
      connections_ = std::move(connections);
      accepting_callbacks_ = true;
      running_ = true;
    }
  }

  void stop() noexcept {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);

    MIDIClientRef client = 0;
    MIDIPortRef input_port = 0;
    {
      std::lock_guard state_lock(state_mutex_);
      if (!running_) {
        return;
      }
      accepting_callbacks_ = false;
      running_ = false;
      client = client_;
      input_port = input_port_;
    }

    for (const auto& connection : connections_) {
      if (input_port != 0 && connection && connection->source != 0) {
        MIDIPortDisconnectSource(input_port, connection->source);
      }
    }
    if (input_port != 0) {
      MIDIPortDispose(input_port);
    }
    if (client != 0) {
      MIDIClientDispose(client);
    }

    {
      std::unique_lock state_lock(state_mutex_);
      callback_condition_.wait(state_lock,
                               [this] { return active_callbacks_ == 0; });
      client_ = 0;
      input_port_ = 0;
      connections_.clear();
    }
  }

  bool is_running() const noexcept {
    std::lock_guard lock(state_mutex_);
    return running_;
  }

 private:
  struct ParsedMessage {
    std::array<std::uint8_t, 3> bytes{};
    std::size_t size = 0;
  };

  struct ParserState {
    std::mutex mutex;
    std::uint8_t running_status = 0;
    std::uint8_t current_status = 0;
    std::array<std::uint8_t, 2> data{};
    std::size_t expected_data = 0;
    std::size_t collected_data = 0;
    bool in_system_exclusive = false;
  };

  struct SourceConnection {
    MidiInputHub* hub = nullptr;
    MIDIEndpointRef source = 0;
    ParserState parser;
  };

  class CallbackGuard {
   public:
    explicit CallbackGuard(MidiInputHub& hub) noexcept
        : hub_(&hub), entered_(hub.begin_callback()) {}

    ~CallbackGuard() {
      if (entered_) {
        hub_->end_callback();
      }
    }

    explicit operator bool() const noexcept { return entered_; }

   private:
    MidiInputHub* hub_;
    bool entered_;
  };

  static void midi_read_proc(const MIDIPacketList* packet_list, void*,
                             void* source_connection) noexcept {
    auto* connection = static_cast<SourceConnection*>(source_connection);
    if (packet_list == nullptr || connection == nullptr ||
        connection->hub == nullptr) {
      return;
    }

    CallbackGuard callback(*connection->hub);
    if (!callback) {
      return;
    }

    const MIDIPacket* packet = &packet_list->packet[0];
    for (UInt32 index = 0; index < packet_list->numPackets; ++index) {
      connection->hub->parse_packet(connection->parser, packet->data,
                                    packet->length);
      packet = MIDIPacketNext(packet);
    }
  }

  bool begin_callback() noexcept {
    std::lock_guard lock(state_mutex_);
    if (!accepting_callbacks_) {
      return false;
    }
    ++active_callbacks_;
    return true;
  }

  void end_callback() noexcept {
    {
      std::lock_guard lock(state_mutex_);
      --active_callbacks_;
    }
    callback_condition_.notify_all();
  }

  static std::size_t data_bytes_for_status(std::uint8_t status) noexcept {
    switch (status & 0xF0) {
      case 0x80:
      case 0x90:
      case 0xA0:
      case 0xB0:
      case 0xE0:
        return 2;
      case 0xC0:
      case 0xD0:
        return 1;
      default:
        break;
    }

    switch (status) {
      case 0xF1:
      case 0xF3:
        return 1;
      case 0xF2:
        return 2;
      case 0xF6:
        return 0;
      default:
        return 0;
    }
  }

  void parse_packet(ParserState& parser, const Byte* data,
                    UInt16 length) noexcept {
    std::vector<ParsedMessage> parsed;
    parsed.reserve(static_cast<std::size_t>(length) / 2 + 1);

    {
      std::lock_guard parser_lock(parser.mutex);
      for (UInt16 index = 0; index < length; ++index) {
        const std::uint8_t byte = static_cast<std::uint8_t>(data[index]);

        // MIDI real-time bytes can appear anywhere and do not disturb
        // running status or a partially collected channel message.
        if (byte >= 0xF8) {
          ParsedMessage message;
          message.bytes[0] = byte;
          message.size = 1;
          parsed.push_back(message);
          continue;
        }

        if (parser.in_system_exclusive) {
          if (byte == 0xF7) {
            parser.in_system_exclusive = false;
          }
          continue;
        }

        if ((byte & 0x80) != 0) {
          parser.collected_data = 0;
          parser.current_status = 0;

          if (byte == 0xF0) {
            parser.in_system_exclusive = true;
            parser.running_status = 0;
            continue;
          }
          if (byte == 0xF7) {
            parser.running_status = 0;
            continue;
          }

          parser.current_status = byte;
          parser.expected_data = data_bytes_for_status(byte);
          if (byte < 0xF0) {
            parser.running_status = byte;
          } else {
            parser.running_status = 0;
          }

          if (parser.expected_data == 0) {
            ParsedMessage message;
            message.bytes[0] = byte;
            message.size = 1;
            parsed.push_back(message);
            parser.current_status = 0;
          }
          continue;
        }

        if (parser.current_status == 0) {
          if (parser.running_status == 0) {
            continue;
          }
          parser.current_status = parser.running_status;
          parser.expected_data = data_bytes_for_status(parser.current_status);
          parser.collected_data = 0;
        }

        if (parser.collected_data < parser.data.size()) {
          parser.data[parser.collected_data++] = byte;
        }

        if (parser.collected_data == parser.expected_data) {
          ParsedMessage message;
          message.bytes[0] = parser.current_status;
          for (std::size_t offset = 0; offset < parser.expected_data;
               ++offset) {
            message.bytes[offset + 1] = parser.data[offset];
          }
          message.size = parser.expected_data + 1;
          parsed.push_back(message);
          parser.current_status = 0;
          parser.collected_data = 0;
        }
      }
    }

    for (const ParsedMessage& message : parsed) {
      dispatch(MidiMessage(message.bytes.data(), message.size));
    }
  }

  void dispatch(MidiMessage message) noexcept {
    std::vector<std::shared_ptr<HandlerSlot>> handlers;
    {
      std::lock_guard lock(state_mutex_);
      handlers.reserve(handlers_.size());
      for (const auto& weak_slot : handlers_) {
        if (auto slot = weak_slot.lock()) {
          handlers.push_back(std::move(slot));
        }
      }
    }

    for (const auto& handler : handlers) {
      handler->invoke(message);
    }
  }

  void erase_disabled_handlers_locked() {
    handlers_.erase(
        std::remove_if(handlers_.begin(), handlers_.end(),
                       [](const std::weak_ptr<HandlerSlot>& weak_slot) {
                         const auto slot = weak_slot.lock();
                         return !slot || !slot->is_enabled();
                       }),
        handlers_.end());
  }

  mutable std::mutex lifecycle_mutex_;
  mutable std::mutex state_mutex_;
  std::condition_variable callback_condition_;
  bool running_ = false;
  bool accepting_callbacks_ = false;
  std::size_t active_callbacks_ = 0;

  MIDIClientRef client_ = 0;
  MIDIPortRef input_port_ = 0;
  std::vector<std::unique_ptr<SourceConnection>> connections_;
  std::vector<std::weak_ptr<HandlerSlot>> handlers_;
};

#endif
