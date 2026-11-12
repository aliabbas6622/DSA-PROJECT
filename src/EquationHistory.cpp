#include "GraphX/EquationHistory.h"

#include <iostream>

namespace graphx {

EquationHistory::Node::Node(const std::string& equationValue,
                            const std::string& postfixValue,
                            const std::string& fileNameValue)
    : equation(equationValue), postfix(postfixValue), fileName(fileNameValue),
      next(NULL) {}

EquationHistory::EquationHistory() : head_(NULL), tail_(NULL), size_(0) {}

EquationHistory::~EquationHistory() { clear(); }

void EquationHistory::add(const std::string& equation, const std::string& postfix, const std::string& fileName) {
    Node* node = new Node(equation, postfix, fileName);
    if (tail_ == NULL) {
        head_ = node;
    } else {
        tail_->next = node;
    }
    tail_ = node;
    ++size_;
}

void EquationHistory::print() const {
    if (head_ == NULL) {
        std::cout << "No equations have been graphed yet.\n";
        return;
    }
    std::size_t index = 1;
    for (const Node* node = head_; node != NULL; node = node->next) {
        std::cout << index++ << ". " << node->equation << "\n"
                  << "   Postfix: " << node->postfix << "\n"
                  << "   Graph: " << node->fileName << "\n";
    }
}

void EquationHistory::clear() {
    while (head_ != NULL) {
        Node* next = head_->next;
        delete head_;
        head_ = next;
    }
    tail_ = NULL;
    size_ = 0;
}

} // namespace graphx
