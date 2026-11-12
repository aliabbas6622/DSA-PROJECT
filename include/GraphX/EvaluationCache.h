#ifndef GRAPHX_EVALUATION_CACHE_H
#define GRAPHX_EVALUATION_CACHE_H

#include <cstddef>
#include <cstdint>
#include <string>

namespace graphx {

class EvaluationCache {
public:
    EvaluationCache();
    ~EvaluationCache();

    bool find(const std::string& expression, double x, double& value);
    void insert(const std::string& expression, double x, double value);
    std::size_t hits() const { return hits_; }
    std::size_t misses() const { return misses_; }

private:
    struct Entry {
        Entry() : occupied(false), xBits(0), value(0.0) {}
        bool occupied;
        std::string expression;
        std::uint64_t xBits;
        double value;
    };

    static std::uint64_t bitsOf(double value);
    static std::size_t hash(const std::string& expression,
                            std::uint64_t xBits);
    void grow();
    void place(Entry* table, std::size_t capacity, const Entry& entry);

    Entry* entries_;
    std::size_t capacity_;
    std::size_t size_;
    std::size_t hits_;
    std::size_t misses_;
};

} // namespace graphx

#endif
