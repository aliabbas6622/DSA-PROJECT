#include "ConsoleApp.h"

#include "GraphX/EquationHistory.h"
#include "GraphX/EvaluationCache.h"
#include "GraphX/Expression.h"
#include "GraphX/GraphSampler.h"
#include "SvgRenderer.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace graphx {
namespace ui {

namespace {

std::string trim(const std::string& text) {
    const std::string::size_type first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const std::string::size_type last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string extractExpression(const std::string& input) {
    const std::string::size_type equals = input.find('=');
    if (equals == std::string::npos) return trim(input);
    const std::string left = trim(input.substr(0, equals));
    if (left != "y") {
        throw std::runtime_error("Use y = expression, or enter an expression in x directly.");
    }
    if (input.find('=', equals + 1) != std::string::npos) {
        throw std::runtime_error("An equation may contain only one equals sign.");
    }
    return trim(input.substr(equals + 1));
}

void printHelp() {
    std::cout
        << "GraphX - Mathematical Equation Grapher\n"
        << "Enter an expression in x or use y = expression.\n"
        << "Commands:\n"
        << "  plot <expression>  Graph an expression and write graph.svg\n"
        << "  range <min> <max>  Set the x and y viewing range (default -10 10)\n"
        << "  history            Show equations graphed this session\n"
        << "  clear              Clear the equation history\n"
        << "  help               Show this help\n"
        << "  exit               Quit GraphX\n"
        << "Operators: + - * / ^ and parentheses\n"
        << "Functions: sin cos tan sqrt log ln abs exp; constants: pi, e\n";
}

} // namespace

int run() {
    GraphSampler sampler(-10.0, 10.0);
    EvaluationCache cache;
    EquationHistory history;

    std::cout << "GraphX - Mathematical Equation Grapher\n"
              << "Type 'help' for commands. Graphs are saved as graph.svg.\n";
    std::string line;
    while (std::cout << "graphx> " && std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        const std::string::size_type separator = line.find_first_of(" \t");
        const std::string command = separator == std::string::npos ?
                                    line : line.substr(0, separator);
        const std::string argument = separator == std::string::npos ?
                                     "" : trim(line.substr(separator + 1));
        if (command == "exit" || command == "quit") {
            break;
        }
        if (command == "help") {
            printHelp();
            continue;
        }
        if (command == "history") {
            history.print();
            continue;
        }
        if (command == "clear") {
            history.clear();
            std::cout << "Equation history cleared.\n";
            continue;
        }
        if (command == "range") {
            std::istringstream range(argument);
            double minimum = 0.0;
            double maximum = 0.0;
            std::string extra;
            if (!(range >> minimum >> maximum) || (range >> extra)) {
                std::cout << "Usage: range <min> <max>\n";
                continue;
            }
            try {
                sampler.setRange(minimum, maximum);
                std::cout << "Graph range set to [" << minimum << ", "
                          << maximum << "].\n";
            } catch (const std::runtime_error& error) {
                std::cout << "Error: " << error.what() << "\n";
            }
            continue;
        }

        std::string equation;
        if (command == "plot") {
            equation = argument;
        } else {
            equation = line;
        }

        try {
            equation = extractExpression(equation);
            Expression expression(equation);
            DynamicArray<GraphSample> samples;
            const std::size_t invalid = sampler.sample(expression, cache, samples);
            const std::string fileName = "graph.svg";
            writeSvg(expression, samples, sampler.minimum(), sampler.maximum(),
                     fileName);
            history.add(equation, expression.postfix(), fileName);
            std::cout << "Graph saved to " << fileName << " (postfix: "
                      << expression.postfix() << ").\n";
            if (invalid != 0) {
                std::cout << invalid
                          << " sample point(s) were outside the real-valued visible range.\n";
            }
            std::cout << "Evaluation cache: " << cache.hits() << " hit(s), "
                      << cache.misses() << " miss(es).\n";
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}

} // namespace ui
} // namespace graphx
