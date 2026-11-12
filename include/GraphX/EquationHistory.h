#ifndef GRAPHX_EQUATION_HISTORY_H
#define GRAPHX_EQUATION_HISTORY_H

#include <cstddef>
#include <string>

namespace graphx {

class EquationHistory {
public:
    EquationHistory();
    ~EquationHistory();

    void add(const std::string& equation, const std::string& postfix,
             const std::string& fileName);
    void print() const;
    void clear();
    std::size_t size() const { return size_; }

private:
    struct Node {
        Node(const std::string& equationValue,
             const std::string& postfixValue,
             const std::string& fileNameValue);
        std::string equation;
        std::string postfix;
        std::string fileName;
        Node* next;
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;
};

} // namespace graphx

#endif
