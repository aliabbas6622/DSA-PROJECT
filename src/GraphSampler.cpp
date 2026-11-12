#include "GraphX/GraphSampler.h"
#include "GraphX/EvaluationCache.h"

#include <cmath>
#include <stdexcept>

namespace graphx {

namespace {

const std::size_t kSamples = 1200;

} // namespace

GraphSampler::GraphSampler(double minimum, double maximum)
    : minimum_(minimum), maximum_(maximum) {
    setRange(minimum, maximum);
}

void GraphSampler::setRange(double minimum, double maximum) {
    if (!std::isfinite(minimum) || !std::isfinite(maximum) ||
        minimum >= maximum || maximum - minimum > 1.0e12) {
        throw std::runtime_error("Range must be finite, increasing, and no wider than 1e12.");
    }
    minimum_ = minimum;
    maximum_ = maximum;
}

std::size_t GraphSampler::sample(
    const Expression& expression,
    EvaluationCache& cache,
    DynamicArray<GraphSample>& samples) const {
    samples.clear();
    std::size_t invalid = 0;

    for (std::size_t i = 0; i <= kSamples; ++i) {
        const double x = minimum_ + (maximum_ - minimum_) *
                                   static_cast<double>(i) / kSamples;
        try {
            double y = 0.0;
            if (!cache.find(expression.source(), x, y)) {
                y = expression.evaluate(x);
                cache.insert(expression.source(), x, y);
            }

            if (!std::isfinite(y) || y < minimum_ || y > maximum_) {
                samples.pushBack(GraphSample(x));
                ++invalid;
            } else {
                samples.pushBack(GraphSample(x, y));
            }
        } catch (const std::runtime_error&) {
            samples.pushBack(GraphSample(x));
            ++invalid;
        }
    }

    return invalid;
}

} // namespace graphx
