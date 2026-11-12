#ifndef GRAPHX_EXPRESSION_H
#define GRAPHX_EXPRESSION_H

#include "GraphX/DynamicArray.h"
#include <string>

namespace graphx {

struct ExpressionNode {
    enum Kind { Number, Variable, UnaryOperator, BinaryOperator, Function };

    ExpressionNode(Kind nodeKind, const std::string& nodeValue);
    ~ExpressionNode();

    Kind kind;
    std::string value;
    double number;
    ExpressionNode* left;
    ExpressionNode* right;
};

class Expression {
public:
    explicit Expression(const std::string& equation);
    ~Expression();

    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;

    double evaluate(double x) const;
    const std::string& source() const { return source_; }
    const std::string& postfix() const { return postfix_; }

private:
    friend class ExpressionParser;

    std::string source_;
    std::string postfix_;
    ExpressionNode* root_;
};

} // namespace graphx

#endif
