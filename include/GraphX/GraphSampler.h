#ifndef GRAPHX_GRAPH_SAMPLER_H
#define GRAPHX_GRAPH_SAMPLER_H

#include "GraphX/DynamicArray.h"
#include "GraphX/Expression.h"

#include <cstddef>

namespace graphx {

class EvaluationCache;

struct GraphSample {
    GraphSample() : x(0.0), y(0.0), valid(false) {}
    GraphSample(double xValue, double yValue)
        : x(xValue), y(yValue), valid(true) {}
    explicit GraphSample(double xValue)
        : x(xValue), y(0.0), valid(false) {}

    double x;
    double y;
    bool valid;
};

class GraphSampler {
public:
    GraphSampler(double minimum, double maximum);

    void setRange(double minimum, double maximum);
    double minimum() const { return minimum_; }
    double maximum() const { return maximum_; }

    std::size_t sample(const Expression& expression,
                       EvaluationCache& cache,
                       DynamicArray<GraphSample>& samples) const;

private:
    double minimum_;
    double maximum_;
};

} // namespace graphx

#endif
