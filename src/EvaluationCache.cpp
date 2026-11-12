#include "GraphX/EvaluationCache.h"

#include <cstring>

namespace graphx {

EvaluationCache::EvaluationCache()
    : entries_(new Entry[64]), capacity_(64), size_(0), hits_(0), misses_(0) {}

EvaluationCache::~EvaluationCache() { delete[] entries_; }

std::uint64_t EvaluationCache::bitsOf(double value) {
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

std::size_t EvaluationCache::hash(const std::string& expression,
                                  std::uint64_t xBits) {
    std::uint64_t result = 1469598103934665603ULL;
    for (std::size_t i = 0; i < expression.size(); ++i) {
        result ^= static_cast<unsigned char>(expression[i]);
        result *= 1099511628211ULL;
    }
    result ^= xBits;
    result *= 1099511628211ULL;
    return static_cast<std::size_t>(result);
}

bool EvaluationCache::find(const std::string& expression, double x,
                           double& value) {
    const std::uint64_t xBits = bitsOf(x);
    std::size_t index = hash(expression, xBits) % capacity_;
    for (std::size_t checked = 0; checked < capacity_; ++checked) {
        const Entry& entry = entries_[index];
        if (!entry.occupied) {
            ++misses_;
            return false;
        }
        if (entry.xBits == xBits && entry.expression == expression) {
            value = entry.value;
            ++hits_;
            return true;
        }
        index = (index + 1) % capacity_;
    }
    ++misses_;
    return false;
}

void EvaluationCache::place(Entry* table, std::size_t capacity,
                            const Entry& entry) {
    std::size_t index = hash(entry.expression, entry.xBits) % capacity;
    while (table[index].occupied) {
        index = (index + 1) % capacity;
    }
    table[index] = entry;
}

void EvaluationCache::grow() {
    const std::size_t nextCapacity = capacity_ * 2;
    Entry* replacement = new Entry[nextCapacity];
    for (std::size_t i = 0; i < capacity_; ++i) {
        if (entries_[i].occupied) {
            place(replacement, nextCapacity, entries_[i]);
        }
    }
    delete[] entries_;
    entries_ = replacement;
    capacity_ = nextCapacity;
}

void EvaluationCache::insert(const std::string& expression, double x,
                             double value) {
    if ((size_ + 1) * 10 >= capacity_ * 7) {
        grow();
    }
    Entry entry;
    entry.occupied = true;
    entry.expression = expression;
    entry.xBits = bitsOf(x);
    entry.value = value;

    std::size_t index = hash(expression, entry.xBits) % capacity_;
    while (entries_[index].occupied) {
        if (entries_[index].xBits == entry.xBits &&
            entries_[index].expression == expression) {
            entries_[index].value = value;
            return;
        }
        index = (index + 1) % capacity_;
    }
    entries_[index] = entry;
    ++size_;
}

} // namespace graphx
