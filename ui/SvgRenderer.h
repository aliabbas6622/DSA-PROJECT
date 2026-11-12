#ifndef GRAPHX_UI_SVG_RENDERER_H
#define GRAPHX_UI_SVG_RENDERER_H

#include "GraphX/GraphSampler.h"
#include "GraphX/Expression.h"

#include <string>

namespace graphx {
namespace ui {

void writeSvg(const Expression& expression,
              const DynamicArray<GraphSample>& samples,
              double minimum,
              double maximum,
              const std::string& fileName);

} // namespace ui
} // namespace graphx

#endif
