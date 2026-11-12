#ifndef GRAPHX_STACK_H
#define GRAPHX_STACK_H

#include "GraphX/DynamicArray.h"
#include <stdexcept>

namespace graphx {

template <typename T>
class Stack {
public:
    void push(const T& value) { values_.pushBack(value); }

    T pop() {
        if (values_.empty()) {
            throw std::runtime_error("Invalid expression: unexpected operator or parenthesis.");
        }
        T value = values_[values_.size() - 1];
        values_.popBack();
        return value;
    }

    T& top() {
        if (values_.empty()) {
            throw std::runtime_error("Invalid expression: unexpected operator or parenthesis.");
        }
        return values_[values_.size() - 1];
    }

    const T& top() const {
        if (values_.empty()) {
            throw std::runtime_error("Invalid expression: unexpected operator or parenthesis.");
        }
        return values_[values_.size() - 1];
    }

    bool empty() const { return values_.empty(); }

private:
    DynamicArray<T> values_;
};

} // namespace graphx

#endif
