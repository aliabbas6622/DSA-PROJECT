#ifndef GRAPHX_DYNAMIC_ARRAY_H
#define GRAPHX_DYNAMIC_ARRAY_H

#include <cstddef>
#include <stdexcept>

namespace graphx {

template <typename T>
class DynamicArray {
public:
    DynamicArray() : data_(NULL), size_(0), capacity_(0) {}

    DynamicArray(const DynamicArray& other)
        : data_(NULL), size_(0), capacity_(0) {
        reserve(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray copy(other);
            swap(copy);
        }
        return *this;
    }

    ~DynamicArray() { delete[] data_; }

    void pushBack(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void popBack() {
        if (size_ == 0) {
            throw std::out_of_range("Cannot remove an item from an empty array.");
        }
        --size_;
    }

    T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Array index is out of range.");
        }
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Array index is out of range.");
        }
        return data_[index];
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() { size_ = 0; }

    void swap(DynamicArray& other) {
        T* data = data_;
        data_ = other.data_;
        other.data_ = data;

        std::size_t value = size_;
        size_ = other.size_;
        other.size_ = value;

        value = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = value;
    }

private:
    void reserve(std::size_t capacity) {
        if (capacity <= capacity_) {
            return;
        }

        T* replacement = new T[capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            replacement[i] = data_[i];
        }
        delete[] data_;
        data_ = replacement;
        capacity_ = capacity;
    }

    T* data_;
    std::size_t size_;
    std::size_t capacity_;
};

} // namespace graphx

#endif
