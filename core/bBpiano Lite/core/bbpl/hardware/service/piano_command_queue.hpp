#ifndef BBPL_PIANO_COMMAND_QUEUE_HPP
#define BBPL_PIANO_COMMAND_QUEUE_HPP

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

enum class PianoCommandType : std::uint8_t {
  note_on,
  note_off,
  note_aftertouch,
  soft_pedal,
  harmonic_pedal,
  sostenuto_pedal,
  sustain_pedal,
  all_silence,
};

struct PianoCommand {
  PianoCommandType type = PianoCommandType::all_silence;
  int note = 0;
  double value = 0.0;
};

// Bounded allocation-free multi-producer/single-consumer queue. Producers are
// CoreMIDI, CLI, and service threads; the Core Audio render thread is the only
// consumer. Capacity must be a power of two.
template <std::size_t Capacity>
class PianoCommandQueue {
  static_assert(Capacity >= 2 && (Capacity & (Capacity - 1)) == 0);
  static_assert(std::is_trivially_copyable_v<PianoCommand>);

 public:
  PianoCommandQueue() noexcept {
    for (std::size_t index = 0; index < Capacity; ++index) {
      slots_[index].sequence.store(index, std::memory_order_relaxed);
    }
  }

  PianoCommandQueue(const PianoCommandQueue&) = delete;
  PianoCommandQueue& operator=(const PianoCommandQueue&) = delete;

  bool try_push(PianoCommand command) noexcept {
    std::size_t position = enqueue_position_.load(std::memory_order_relaxed);
    for (;;) {
      Slot& slot = slots_[position & (Capacity - 1)];
      const std::size_t sequence =
          slot.sequence.load(std::memory_order_acquire);
      const auto difference = static_cast<std::intptr_t>(sequence) -
                              static_cast<std::intptr_t>(position);
      if (difference == 0) {
        if (enqueue_position_.compare_exchange_weak(
                position, position + 1, std::memory_order_relaxed)) {
          slot.command = command;
          slot.sequence.store(position + 1, std::memory_order_release);
          return true;
        }
      } else if (difference < 0) {
        return false;
      } else {
        position = enqueue_position_.load(std::memory_order_relaxed);
      }
    }
  }

  bool try_pop(PianoCommand& command) noexcept {
    Slot& slot = slots_[dequeue_position_ & (Capacity - 1)];
    const std::size_t sequence = slot.sequence.load(std::memory_order_acquire);
    const auto difference = static_cast<std::intptr_t>(sequence) -
                            static_cast<std::intptr_t>(dequeue_position_ + 1);
    if (difference != 0) {
      return false;
    }

    command = slot.command;
    slot.sequence.store(dequeue_position_ + Capacity,
                        std::memory_order_release);
    ++dequeue_position_;
    return true;
  }

  void clear() noexcept {
    PianoCommand command;
    while (try_pop(command)) {
    }
  }

 private:
  struct Slot {
    std::atomic<std::size_t> sequence{0};
    PianoCommand command{};
  };

  alignas(64) std::array<Slot, Capacity> slots_{};
  alignas(64) std::atomic<std::size_t> enqueue_position_{0};
  alignas(64) std::size_t dequeue_position_ = 0;
};

class PianoCommandBus {
 public:
  bool post(PianoCommand command) noexcept {
    if (queue_.try_push(command)) {
      return true;
    }
    // Dropping note-off or pedal-up can leave a stuck state. Overflow therefore
    // requests a deterministic reset on the audio thread instead.
    reset_requested_.store(true, std::memory_order_release);
    return false;
  }

  template <class Consumer>
  void drain(Consumer&& consume) {
    PianoCommand command;
    if (reset_requested_.exchange(false, std::memory_order_acq_rel)) {
      while (queue_.try_pop(command)) {
      }
      consume(PianoCommand{PianoCommandType::all_silence, 0, 0.0});
    }
    while (queue_.try_pop(command)) {
      consume(command);
    }
  }

  // Call only from the single consumer after the AudioUnit callback gate has
  // closed. Producers must already belong to stopped service objects.
  void clear() noexcept {
    queue_.clear();
    reset_requested_.store(false, std::memory_order_release);
  }

 private:
  PianoCommandQueue<4096> queue_;
  std::atomic<bool> reset_requested_{false};
};

#endif
