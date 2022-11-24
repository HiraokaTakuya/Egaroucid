/*
    Egaroucid Project

    @file spinlock.hpp
        Spinlock
    @date 2021-2022
    @author Takuto Yamana (a.k.a. Nyanyan)
    @license GPL-3.0 license
    @notice I referred to codes written by others
*/

#pragma once
#include <atomic>
#include <thread>
#include <mutex>

// original: https://rigtorp.se/spinlock/
// modified by Nyanyan
struct Spinlock {
    std::atomic<bool> lock_ = {0};

    void lock(){
        for (;;) {
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            //while (lock_.load(std::memory_order_relaxed)) {
            //    __builtin_ia32_pause();
            //}
        }
    }

    bool try_lock() noexcept {
        return !lock_.load(std::memory_order_relaxed) && !lock_.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        lock_.store(false, std::memory_order_release);
    }
};
// end of modification

/*
// original: https://cpprefjp.github.io/reference/atomic/atomic_flag.html
// modified by Nyanyan
class Spinlock {
    private:
        std::atomic_flag state_;

    public:
        Spinlock() : state_(ATOMIC_FLAG_INIT) {}

        void lock(){
            while (state_.test_and_set(std::memory_order_acquire)) {
            }
        }

        void unlock(){
            state_.clear(std::memory_order_release);
        }
};
// end of modification
*/