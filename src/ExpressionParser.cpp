#include "GraphX/ExpressionParser.h"
#include "GraphX/Stack.h"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace graphx {

namespace {

struct Token {
    enum Type { Number, Variable, Operator, Function, LeftParen, RightParen };
    Token() : type(Operator), number(0.0) {}
    Token(Type tokenType, const std::string& tokenText, double numberValue = 0.0)
        : type(tokenType), text(tokenText), number(numberValue) {}
    Type type;
    std::string text;
    double number;
};

bool isFunction(const std::string& name) {
    return name == "sin" || name == "cos" || name == "tan" ||
           name == "sqrt" || name == "log" || name == "ln" ||
           name == "abs" || name == "exp";
}

DynamicArray<Token> tokenize(const std::string& source) {
    DynamicArray<Token> tokens;
    bool expectsOperand = true;
    std::size_t i = 0;
    while (i < source.size()) {
        const unsigned char current = static_cast<unsigned char>(source[i]);
        if (std::isspace(current)) {
            ++i;
            continue;
        }

        if (std::isdigit(current) || source[i] == '.') {
            char* end = NULL;
            const char* start = source.c_str() + i;
            const double value = std::strtod(start, &end);
            if (end == start) {
                throw std::runtime_error("Invalid number in expression.");
            }
            const std::size_t length = static_cast<std::size_t>(end - start);
            tokens.pushBack(Token(Token::Number, source.substr(i, length), value));
            i += length;
            expectsOperand = false;
            continue;
        }

        if (std::isalpha(current)) {
            const std::size_t start = i;
            while (i < source.size() &&
                   std::isalpha(static_cast<unsigned char>(source[i]))) {
                ++i;
            }
            const std::string name = source.substr(start, i - start);
            if (name == "x" || name == "pi" || name == "e") {
                tokens.pushBack(Token(Token::Variable, name));
                expectsOperand = false;
            } else if (isFunction(name)) {
                tokens.pushBack(Token(Token::Function, name));
                expectsOperand = true;
            } else {
                throw std::runtime_error("Unknown name: " + name);
            }
            continue;
        }

        if (source[i] == '(') {
            tokens.pushBack(Token(Token::LeftParen, "("));
            expectsOperand = true;
            ++i;
            continue;
        }
        if (source[i] == ')') {
            tokens.pushBack(Token(Token::RightParen, ")"));
            expectsOperand = false;
            ++i;
            continue;
        }

        const char symbol = source[i];
        if (symbol == '+' || symbol == '-' || symbol == '*' ||
            symbol == '/' || symbol == '^') {
            std::string operation(1, symbol);
            if (expectsOperand && (symbol == '+' || symbol == '-')) {
                operation = symbol == '-' ? "u-" : "u+";
            } else if (expectsOperand) {
                throw std::runtime_error("An operator is missing its left operand.");
            }
            tokens.pushBack(Token(Token::Operator, operation));
            expectsOperand = true;
            ++i;
            continue;
        }

        throw std::runtime_error(std::string("Unexpected character: ") + source[i]);
    }

    if (tokens.empty()) {
        throw std::runtime_error("Enter an expression to graph.");
    }
    if (expectsOperand) {
        throw std::runtime_error("The expression ends with an incomplete operation.");
    }
    return tokens;
}

int precedence(const std::string& operation) {
    if (operation == "u-" || operation == "u+") return 4;
    if (operation == "^") return 5;
    if (operation == "*" || operation == "/") return 3;
    return 2;
}

bool isRightAssociative(const std::string& operation) {
    return operation == "^" || operation == "u-" || operation == "u+";
}

DynamicArray<Token> toPostfix(const DynamicArray<Token>& input) {
    DynamicArray<Token> output;
    Stack<Token> operators;

    for (std::size_t i = 0; i < input.size(); ++i) {
        const Token& token = input[i];
        if (token.type == Token::Number || token.type == Token::Variable) {
            output.pushBack(token);
        } else if (token.type == Token::Function) {
            operators.push(token);
        } else if (token.type == Token::Operator) {
            while (!operators.empty() &&
                   (operators.top().type == Token::Function ||
                    (operators.top().type == Token::Operator &&
                     (precedence(operators.top().text) > precedence(token.text) ||
                      (precedence(operators.top().text) == precedence(token.text) &&
                       !isRightAssociative(token.text)))))) {
                output.pushBack(operators.pop());
            }
            operators.push(token);
        } else if (token.type == Token::LeftParen) {
            operators.push(token);
        } else {
            bool foundLeftParen = false;
            while (!operators.empty()) {
                Token top = operators.pop();
                if (top.type == Token::LeftParen) {
                    foundLeftParen = true;
                    break;
                }
                output.pushBack(top);
            }
            if (!foundLeftParen) {
                throw std::runtime_error("Mismatched closing parenthesis.");
            }
            if (!operators.empty() && operators.top().type == Token::Function) {
                output.pushBack(operators.pop());
            }
        }
    }

    while (!operators.empty()) {
        Token top = operators.pop();
        if (top.type == Token::LeftParen || top.type == Token::RightParen) {
            throw std::runtime_error("Mismatched parenthesis.");
        }
        output.pushBack(top);
    }
    return output;
}

ExpressionNode* buildTree(const DynamicArray<Token>& postfix) {
    Stack<ExpressionNode*> nodes;
    try {
        for (std::size_t i = 0; i < postfix.size(); ++i) {
            const Token& token = postfix[i];
            if (token.type == Token::Number) {
                ExpressionNode* node = new ExpressionNode(ExpressionNode::Number, token.text);
                node->number = token.number;
                nodes.push(node);
            } else if (token.type == Token::Variable) {
                nodes.push(new ExpressionNode(ExpressionNode::Variable, token.text));
            } else if (token.type == Token::Function ||
                       (token.type == Token::Operator &&
                        (token.text == "u-" || token.text == "u+"))) {
                if (nodes.empty()) {
                    throw std::runtime_error("A function or unary operator is missing its operand.");
                }
                ExpressionNode* node = new ExpressionNode(
                    token.type == Token::Function ? ExpressionNode::Function :
                                                   ExpressionNode::UnaryOperator,
                    token.text);
                node->left = nodes.pop();
                nodes.push(node);
            } else if (token.type == Token::Operator) {
                if (nodes.empty()) {
                    throw std::runtime_error("An operator is missing an operand.");
                }
                ExpressionNode* right = nodes.pop();
                if (nodes.empty()) {
                    delete right;
                    throw std::runtime_error("An operator is missing its left operand.");
                }
                ExpressionNode* node = new ExpressionNode(ExpressionNode::BinaryOperator,
                                                          token.text);
                node->left = nodes.pop();
                node->right = right;
                nodes.push(node);
            }
        }
        if (nodes.empty()) {
            throw std::runtime_error("Could not build an expression tree.");
        }
        ExpressionNode* root = nodes.pop();
        if (!nodes.empty()) {
            delete root;
            throw std::runtime_error("Invalid expression: two adjacent values need an operator.");
        }
        return root;
    } catch (...) {
        while (!nodes.empty()) {
            delete nodes.pop();
        }
        throw;
    }
}

std::string tokenText(const Token& token) {
    if (token.type == Token::Number || token.type == Token::Variable ||
        token.type == Token::Function) {
        return token.text;
    }
    return token.text;
}

} // namespace

ExpressionNode* ExpressionParser::parse(const std::string& expression,
                                        std::string& postfixText) {
    const DynamicArray<Token> tokens = tokenize(expression);
    const DynamicArray<Token> postfix = toPostfix(tokens);
    std::ostringstream rendered;
    for (std::size_t i = 0; i < postfix.size(); ++i) {
        if (i != 0) rendered << ' ';
        rendered << tokenText(postfix[i]);
    }
    postfixText = rendered.str();
    return buildTree(postfix);
}

} // namespace graphx
