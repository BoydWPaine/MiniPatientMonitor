#ifndef MPM_HOST_RING_BUFFER_H
#define MPM_HOST_RING_BUFFER_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mpm::host {

template <size_t Capacity>
class RingBuffer {
public:
    void push(int32_t sample)
    {
        buffer_[head_] = sample;
        head_ = (head_ + 1U) % Capacity;
        if (count_ < Capacity) {
            ++count_;
        }
    }

    size_t size() const { return count_; }

    void copy_ordered(std::vector<int32_t>& out) const
    {
        out.clear();
        out.reserve(count_);
        const size_t start = (head_ + Capacity - count_) % Capacity;
        for (size_t i = 0; i < count_; ++i) {
            out.push_back(buffer_[(start + i) % Capacity]);
        }
    }

    void clear()
    {
        head_ = 0;
        count_ = 0;
    }

private:
    std::array<int32_t, Capacity> buffer_{};
    size_t head_ = 0;
    size_t count_ = 0;
};

constexpr size_t kRingBufferCapacity = 120;

}  // namespace mpm::host

#endif