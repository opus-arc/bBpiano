#ifndef BBPL_AUDIO_CALLBACK_GATE_HPP
#define BBPL_AUDIO_CALLBACK_GATE_HPP

#include <atomic>
#include <cstdint>

// Allocation-free admission barrier for a real-time callback. open() is called
// immediately before AudioOutputUnitStart. close_and_wait() is called before
// callback dependencies are released.
class AudioCallbackGate {
 public:
  class Guard {
   public:
    explicit Guard(AudioCallbackGate& gate) noexcept
        : gate_(&gate), entered_(gate.try_enter()) {}

    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    ~Guard() {
      if (entered_) {
        gate_->leave();
      }
    }

    explicit operator bool() const noexcept { return entered_; }

   private:
    AudioCallbackGate* gate_;
    bool entered_;
  };

  AudioCallbackGate() noexcept = default;
  AudioCallbackGate(const AudioCallbackGate&) = delete;
  AudioCallbackGate& operator=(const AudioCallbackGate&) = delete;

  void open() noexcept { state_.store(0, std::memory_order_release); }

  void close_and_wait() noexcept {
    std::uint32_t state =
        state_.fetch_or(k_stopping_bit, std::memory_order_acq_rel) |
        k_stopping_bit;
    while ((state & k_count_mask) != 0) {
      state_.wait(state, std::memory_order_acquire);
      state = state_.load(std::memory_order_acquire);
    }
  }

  bool is_open() const noexcept {
    return (state_.load(std::memory_order_acquire) & k_stopping_bit) == 0;
  }

 private:
  static constexpr std::uint32_t k_stopping_bit = 0x8000'0000U;
  static constexpr std::uint32_t k_count_mask = ~k_stopping_bit;

  bool try_enter() noexcept {
    std::uint32_t state = state_.load(std::memory_order_acquire);
    while ((state & k_stopping_bit) == 0) {
      if ((state & k_count_mask) == k_count_mask) {
        return false;
      }
      if (state_.compare_exchange_weak(state, state + 1,
                                       std::memory_order_acq_rel,
                                       std::memory_order_acquire)) {
        return true;
      }
    }
    return false;
  }

  void leave() noexcept {
    state_.fetch_sub(1, std::memory_order_release);
    state_.notify_all();
  }

  std::atomic<std::uint32_t> state_{k_stopping_bit};
};

#endif
