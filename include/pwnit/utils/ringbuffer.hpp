#pragma once

#include <vector>
#include <utility>

namespace pwnit::utils
{
    
template <typename T>
class RingBuffer
{
private:
    std::vector<T> data_;
    int capacity_;
    int head_;
    int size_;

public:
    explicit RingBuffer(int capacity)
        : data_(capacity),
          capacity_(capacity),
          head_(0),
          size_(0) {}

    T& operator[](std::size_t index) noexcept
    {
        return data_[(head_ + index) % capacity_];
    }

    const T& operator[](std::size_t index) const noexcept
    {
        return data_[(head_ + index) % capacity_];
    }

    void push(const T& value) noexcept
    {
        if (capacity_ == 0)
            return;

        data_[(head_ + size_) % capacity_] = value;

        if (size_ < capacity_)
            size_++;
        else
            head_ = (head_ + 1) % capacity_;
    }

    void push(T&& value) noexcept
    {
        if (capacity_ == 0)
            return;

        data_[(head_ + size_) % capacity_] = std::move(value);

        if (size_ < capacity_)
            size_++;
        else
            head_ = (head_ + 1) % capacity_;
    }

    void pop() noexcept
    {
        if (capacity_ == 0)
            return;

        size_--;
        head_ = (head_ + 1) % capacity_;
    }

    T& front() noexcept
    {
        return data_[head_];
    }

    const T& front() const noexcept
    {
        return data_[head_];
    }

    T& back() noexcept
    {
        return data_[(head_ + size_ - 1) % capacity_];
    }

    const T& back() const noexcept
    {
        return data_[(head_ + size_ - 1) % capacity_];
    }

    int size() const noexcept
    {
        return size_;
    }

    int capacity() const noexcept
    {
        return capacity_;
    }

    void clear() noexcept
    {
        size_ = 0;
        data_.clear();
    }
    
    bool empty() const noexcept
    {
        return size_ == 0;
    }

    bool full() const noexcept
    {
        return size_ == capacity_;
    }

    class iterator {
    public:
        iterator(RingBuffer* buffer, std::size_t index)
            : buffer_(buffer), index_(index)
        {}

        T& operator*()
        {
            return (*buffer_)[index_];
        }

        iterator& operator++()
        {
            ++index_;
            return *this;
        }

        bool operator!=(const iterator& other) const
        {
            return index_ != other.index_;
        }

    private:
        RingBuffer* buffer_;
        std::size_t index_;
    };
    
    iterator begin() noexcept
    {
        return iterator(this, 0);
    }

    iterator end() noexcept
    {
        return iterator(this, size_);
    }
};
    
}
