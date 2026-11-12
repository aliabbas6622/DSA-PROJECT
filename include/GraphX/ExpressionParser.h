#ifndef GRAPHX_EXPRESSION_PARSER_H
#define GRAPHX_EXPRESSION_PARSER_H

#include "GraphX/DynamicArray.h"
#include "GraphX/Expression.h"
#include <string>

namespace graphx {

class ExpressionParser {
public:
    static ExpressionNode* parse(const std::string& expression,
                                 std::string& postfixText);
};

} // namespace graphx

#endif
