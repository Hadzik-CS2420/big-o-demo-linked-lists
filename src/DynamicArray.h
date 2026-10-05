// ============================================================================
// DynamicArray -- a minimal growable array of ints, for the Big O demos
// ============================================================================
// The same resize-copy pattern CT 07 builds by hand, with one switch: how much
// to grow when the array is full.
//
//   Growth::Double -- capacity * 2   (what std::vector does)
//   Growth::ByOne  -- capacity + 1   (the "obvious" way, and the slow one)
//
// It also counts its own work -- how many times it reallocated, how many
// elements it copied, and the most memory it ever held at once -- so the space
// demo can report real numbers rather than formulas.
// ============================================================================
#pragma once

#include <cstddef>

enum class Growth { Double, ByOne };

class DynamicArray {
public:
    explicit DynamicArray(Growth growth) : growth_{growth} {}
    ~DynamicArray() { delete[] data_; }

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    // Amortized O(1) when doubling; O(n) every time when growing by one.
    void push_back(int value) {
        if (count_ == capacity_) grow();
        data_[count_++] = value;
    }

    // O(n): every existing element shifts one slot right to make room.
    void insert_front(int value) {
        if (count_ == capacity_) grow();
        for (std::size_t i = count_; i > 0; --i) data_[i] = data_[i - 1];
        data_[0] = value;
        ++count_;
    }

    // O(n): every remaining element shifts one slot left to close the gap.
    void remove_front() {
        if (count_ == 0) return;
        for (std::size_t i = 1; i < count_; ++i) data_[i - 1] = data_[i];
        --count_;
    }

    // O(1): the last element just stops counting -- nothing moves.
    void pop_back() {
        if (count_ > 0) --count_;
    }

    // O(1): one address calculation, base + i * sizeof(int).
    int at(std::size_t i) const { return data_[i]; }

    // O(n): may have to look at every element.
    bool contains(int value) const {
        for (std::size_t i = 0; i < count_; ++i)
            if (data_[i] == value) return true;
        return false;
    }

    std::size_t size() const { return count_; }
    std::size_t capacity() const { return capacity_; }
    std::size_t reallocations() const { return reallocations_; }
    std::size_t elements_copied() const { return copied_; }
    std::size_t peak_bytes() const { return peak_bytes_; }

private:
    // Allocate, copy, free, repoint -- the CT 07 order.
    void grow() {
        std::size_t new_cap = capacity_ == 0 ? 1
                            : growth_ == Growth::Double ? capacity_ * 2
                            : capacity_ + 1;
        int* bigger = new int[new_cap];
        for (std::size_t i = 0; i < count_; ++i) bigger[i] = data_[i];
        // Both blocks are alive right here -- this is the peak.
        std::size_t both = (capacity_ + new_cap) * sizeof(int);
        if (both > peak_bytes_) peak_bytes_ = both;
        copied_ += count_;
        delete[] data_;
        data_ = bigger;
        capacity_ = new_cap;
        ++reallocations_;
    }

    Growth growth_;
    int* data_ = nullptr;
    std::size_t count_ = 0;
    std::size_t capacity_ = 0;
    std::size_t reallocations_ = 0;
    std::size_t copied_ = 0;
    std::size_t peak_bytes_ = 0;
};
