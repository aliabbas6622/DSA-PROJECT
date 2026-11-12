# GraphX — Mathematical Equation Grapher

GraphX is a C++ console application for entering equations in `x`, parsing them
into postfix notation and an expression tree, evaluating the tree, and saving
the graph as a standalone SVG file. It uses no third-party graphics library.

## Build

Requirements: a C++11 compiler and CMake 3.10 or newer.

```text
cmake -S . -B build
cmake --build build
```

Or compile directly with MinGW:

```text
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude -Iui src/*.cpp ui/*.cpp -o graphx
```

## Use

Run `graphx`, then enter an expression such as `x^2 - 3*x + 1` or an equation
such as `y = sin(x)`. GraphX writes the plot to `graph.svg` in the current
directory; open the SVG in a web browser. The default viewing range is
`[-10, 10]` for both axes.

Commands:

* `plot <expression>` — graph an expression.
* `range <min> <max>` — set both axis ranges.
* `history` — list equations and their postfix forms for this session.
* `clear`, `help`, `exit`.

Supported operators are `+`, `-`, `*`, `/`, `^`, unary signs, and parentheses.
Supported functions are `sin`, `cos`, `tan`, `sqrt`, `log`, `ln`, `abs`, and
`exp`; constants are `pi` and `e`.

## Data structures and algorithms

* `DynamicArray` manages parser tokens, stack storage, and sampled graph points.
* `Stack` is used by the shunting-yard infix-to-postfix parser and tree builder.
* The parser creates a pointer-based binary expression tree; evaluation and
  destruction use recursion.
* `GraphSampler` evaluates the expression at evenly spaced `x` values and
  stores the resulting `(x, y)` samples in a dynamic array.
* `EquationHistory` is a singly linked list of plotted equations.
* `EvaluationCache` is an open-addressed hash table keyed by expression and the
  exact floating-point sample, so repeated plots reuse evaluation results.
* The separate `ui/` folder contains the command-line interface and SVG
  presentation code. The SVG writer draws the sample array; it does not
  calculate the equation.

Invalid syntax is reported before plotting. Domain errors such as division by
zero and negative square roots are skipped at the affected graph samples.

## UI and AI assistance

The presentation layer is separated into [ui/](ui/), apart from the parser,
expression tree, dynamic array, stack, linked-list history, evaluation cache,
and graph-sampling code. The SVG styling/layout and command-line interaction
in `ui/` were created with AI assistance; see [ui/README.md](ui/README.md).
The learning guide is in [docs/learning-guide/README.md](docs/learning-guide/README.md).
