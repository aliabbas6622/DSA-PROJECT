#include "GraphX/Expression.h"
#include "GraphX/ExpressionParser.h"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace graphx {

ExpressionNode::ExpressionNode(Kind nodeKind, const std::string& nodeValue)
    : kind(nodeKind), value(nodeValue), number(0.0), left(NULL), right(NULL) {}

ExpressionNode::~ExpressionNode() {
    delete left;
    delete right;
}

Expression::Expression(const std::string& equation)
    : source_(equation), root_(NULL) {
    root_ = ExpressionParser::parse(source_, postfix_);
}

Expression::~Expression() { delete root_; }

static double evaluateNode(const ExpressionNode* node, double x) {
    if (node == NULL) {
        throw std::runtime_error("Invalid expression tree.");
    }

    if (node->kind == ExpressionNode::Number) {
        return node->number;
    }
    if (node->kind == ExpressionNode::Variable) {
        return node->value == "x" ? x : (node->value == "pi" ? std::acos(-1.0) : std::exp(1.0));
    }

    const double left = evaluateNode(node->left, x);
    if (node->kind == ExpressionNode::UnaryOperator) {
        return node->value == "u-" ? -left : left;
    }
    if (node->kind == ExpressionNode::Function) {
        if (node->value == "sin") return std::sin(left);
        if (node->value == "cos") return std::cos(left);
        if (node->value == "tan") return std::tan(left);
        if (node->value == "sqrt") {
            if (left < 0.0) throw std::runtime_error("sqrt is undefined for negative values.");
            return std::sqrt(left);
        }
        if (node->value == "log" || node->value == "ln") {
            if (left <= 0.0) throw std::runtime_error("log is defined only for positive values.");
            return std::log(left);
        }
        if (node->value == "abs") return std::fabs(left);
        if (node->value == "exp") return std::exp(left);
        throw std::runtime_error("Unsupported function: " + node->value);
    }

    const double right = evaluateNode(node->right, x);
    double result = 0.0;
    if (node->value == "+") result = left + right;
    else if (node->value == "-") result = left - right;
    else if (node->value == "*") result = left * right;
    else if (node->value == "/") {
        if (right == 0.0) throw std::runtime_error("Division by zero.");
        result = left / right;
    } else if (node->value == "^") {
        result = std::pow(left, right);
    } else {
        throw std::runtime_error("Unsupported operator: " + node->value);
    }

    if (!std::isfinite(result)) {
        throw std::runtime_error("The expression is outside its real-valued domain.");
    }
    return result;
}

double Expression::evaluate(double x) const {
    const double result = evaluateNode(root_, x);
    if (!std::isfinite(result)) {
        throw std::runtime_error("The expression is outside its real-valued domain.");
    }
    return result;
}

} // namespace graphx
