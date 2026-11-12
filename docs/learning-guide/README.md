# GraphX: Beginner's Guide to the Project

This guide explains what is in GraphX, how the pieces work together, and where
to look when you want to understand or change something. You do not need to
read every file before you can run the program.

## What GraphX does

GraphX is a command-line program. You type a function of `x`, such as
`x^2 - 3*x + 1`, and it:

1. Checks the text and separates it into meaningful pieces (tokens).
2. Converts the usual math notation into postfix notation.
3. Builds a binary expression tree from those tokens.
4. Evaluates that tree at many `x` values.
5. Writes the result as `graph.svg`, which you can open in a web browser.
6. Adds the equation to an in-memory history list.

The program also remembers evaluated sample points in a hash table, so
plotting the same equation over the same range can reuse previous results.

## Project map

```text
DSA PROJECT/
|-- .vscode/
|   `-- tasks.json                 VS Code build task
|-- docs/
|   `-- learning-guide/
|       `-- README.md              This guide
|-- ui/                            Separate, AI-assisted interface and SVG code
|   |-- ConsoleApp.cpp/.h          Terminal commands and session interaction
|   `-- SvgRenderer.cpp/.h          Turns samples into an SVG image
|-- include/
|   `-- GraphX/                    Header files: declarations and class layouts
|-- src/                           Math, data structures, and sampling implementation
|-- CMakeLists.txt                 CMake build instructions
`-- README.md                      Short project overview and quick start
```

### Build and project instructions

- [CMakeLists.txt](../../CMakeLists.txt) tells CMake which C++ files make the
  application and where to find the headers.
- [.vscode/tasks.json](../../.vscode/tasks.json) defines the default VS Code
  build task. It compiles the core and UI source files and adds their header
  paths. Use **Terminal > Run Build Task** (Ctrl+Shift+B).
- [README.md](../../README.md) at the project root has the short build and
  usage instructions.
- [ui/README.md](../../ui/README.md) identifies the separate UI layer and
  describes which parts were created with AI assistance.

### Header files (`include/GraphX/`)

A header (`.h`) describes a class or type: its name, data, and available
functions. The matching `.cpp` file usually contains the function bodies.

- [DynamicArray.h](../../include/GraphX/DynamicArray.h) — a resizable array
  implemented with a pointer and a manually allocated block of memory.
- [Stack.h](../../include/GraphX/Stack.h) — a last-in, first-out stack built
  on `DynamicArray`.
- [Expression.h](../../include/GraphX/Expression.h) — expression-tree nodes
  and the `Expression` class.
- [ExpressionParser.h](../../include/GraphX/ExpressionParser.h) — the parser's
  public entry point.
- [EvaluationCache.h](../../include/GraphX/EvaluationCache.h) — the hash table
  interface for reusing evaluated values.
- [EquationHistory.h](../../include/GraphX/EquationHistory.h) — the linked-list
  history interface.
- [GraphSampler.h](../../include/GraphX/GraphSampler.h) — the sample point
  structure and interface for calculating graph values.

### C++ implementation files (`src/`)

- [main.cpp](../../src/main.cpp) — small program entry point; starts the
  command-line interface in `ui/`.
- [ExpressionParser.cpp](../../src/ExpressionParser.cpp) — tokenizes
  expressions, converts them to postfix, and builds the expression tree.
- [Expression.cpp](../../src/Expression.cpp) — creates and destroys expression
  nodes and recursively evaluates a tree.
- [GraphSampler.cpp](../../src/GraphSampler.cpp) — evaluates the expression at
  many `x` values and stores `(x, y)` samples in `DynamicArray<GraphSample>`.
- [EvaluationCache.cpp](../../src/EvaluationCache.cpp) — implements hashing,
  collision handling, cache lookup, and table growth.
- [EquationHistory.cpp](../../src/EquationHistory.cpp) — adds, displays, and
  clears the linked list of plotted equations.

### Separate UI files (`ui/`)

- [ConsoleApp.h](../../ui/ConsoleApp.h) and
  [ConsoleApp.cpp](../../ui/ConsoleApp.cpp) — command prompt, help, parsing
  user commands, and passing data between the core and graph display.
- [SvgRenderer.h](../../ui/SvgRenderer.h) and
  [SvgRenderer.cpp](../../ui/SvgRenderer.cpp) — take the already-calculated
  sample array and create an SVG image. They do not evaluate the equation.
- [ui/README.md](../../ui/README.md) — explains the UI folder and documents
  that its interface/presentation code was created with AI assistance.

## How one equation travels through the program

For example, enter:

```text
y = x + 2 * 3
```

The console UI removes the optional `y =` part. The parser turns the remaining
expression into tokens:

```text
x    +    2    *    3
```

The parser uses a stack to produce postfix notation:

```text
x 2 3 * +
```

Postfix notation puts each operator after its inputs, so parentheses are not
needed to show the order. The parser then builds this tree:

```text
       +
      / \
     x   *
        / \
       2   3
```

The `Expression` recursively evaluates the tree. The `GraphSampler` chooses
many `x` values and saves each result as a sample in a dynamic array. For
`x = 1`, for example, recursive evaluation gives `1 + (2 * 3) = 7`, so one
stored sample is `(1, 7)`. The UI's SVG writer reads that array and draws it;
it does not redo the math.

## Main functions and classes

### `main.cpp` and the console UI

`main.cpp` is intentionally tiny: it calls `graphx::ui::run()`. The actual
interactive interface is in `ui/ConsoleApp.cpp`.

- `run()` constructs the sampler, cache, and history, then reads commands in a
  loop.
- `trim(text)` removes whitespace at the start and end of text.
- `extractExpression(input)` accepts either an expression or `y = expression`
  and returns just the right-hand expression.
- `printHelp()` prints supported commands and math features.
- `plot` (or simply typing an expression) parses it, asks the sampler for an
  array of sample values, and passes those values to `writeSvg(...)`.
- `range`, `history`, `clear`, `help`, and `exit` handle the other commands.

### `DynamicArray<T>`: growable storage

This template stores items in a dynamically allocated array. `T` is a
placeholder for the type of item, such as a token or `GraphSample` containing
an x value, y value, and validity flag.

- `pushBack(value)` adds an item and grows the allocation when it is full.
- `popBack()` removes the last item.
- `operator[](index)` accesses an item and checks that the index is valid.
- `size()` and `empty()` report how many items are stored.
- `clear()` resets the logical size; `swap()` exchanges two arrays.
- `reserve(capacity)` is private and allocates a larger block, copying existing
  items. Capacity doubles when more space is needed.
- The copy constructor and assignment operator make independent copies.
- The destructor uses `delete[]` to release the allocated block.

This is the project's own dynamic-array implementation; it is used instead of
an STL vector for the main DSA work.

### `Stack<T>`: last in, first out

A stack lets you add and remove items at one end. The last item pushed is the
first one popped.

- `push(value)` adds to the top.
- `pop()` removes and returns the top item.
- `top()` reads the top item without removing it.
- `empty()` checks whether there are any items.

The parser uses stacks to order operators and to assemble the expression tree.

### `ExpressionParser`: understanding the math text

The parser's helper functions are private to `ExpressionParser.cpp`; the
public entry point is `ExpressionParser::parse(expression, postfixText)`.

- `Token` represents one number, variable, operator, function, or parenthesis.
- `isFunction(name)` recognizes supported function names.
- `tokenize(source)` reads the expression character by character and reports
  unknown names, characters, or incomplete expressions.
- `precedence(operator)` assigns operator priority. Higher numbers bind more
  tightly.
- `isRightAssociative(operator)` makes exponentiation group from the right:
  `2^3^2` means `2^(3^2)`.
- `toPostfix(tokens)` uses the shunting-yard algorithm and an operator stack.
- `buildTree(postfix)` uses a stack of node pointers to construct the binary
  expression tree.
- `tokenText(token)` provides the printable token text used in the postfix
  result.
- `parse(...)` performs tokenizing, postfix conversion, and tree construction.

Recognized operators are `+`, `-`, `*`, `/`, and `^`, including unary `+` and
`-`. Recognized functions are `sin`, `cos`, `tan`, `sqrt`, `log`, `ln`, `abs`,
and `exp`. Constants are `pi` and `e`. The variable is `x`.

Write explicit multiplication, for example `2*x`; `2x` is not supported.

### `Expression` and `ExpressionNode`: storing and calculating the equation

An `ExpressionNode` is one node in the tree. It stores a kind, a value, and
pointers to its left and right children. A number or variable is a leaf;
operators and functions connect to their input nodes.

- `ExpressionNode(...)` initializes a node with no children.
- `~ExpressionNode()` recursively deletes the left and right subtrees.
- `Expression(equation)` asks the parser to create the tree and postfix text.
- `~Expression()` deletes the tree's root, which recursively frees the whole
  tree.
- `evaluate(x)` starts recursive calculation at the root.
- `evaluateNode(node, x)` is the recursive helper in `Expression.cpp`. It
  evaluates the left and right subtrees before applying their operator or
  function.
- `source()` and `postfix()` return the original expression and its postfix
  form.

Division by zero, negative square roots, and logarithms of non-positive values
are reported as math errors. During plotting, invalid sample points are skipped
so the rest of the curve can still be drawn.

### `EvaluationCache`: reusing calculations

The cache maps an expression and an exact `x` sample to the resulting `y`.

- `bitsOf(value)` copies a `double`'s bit pattern so the sample can be used as
  part of a hash key.
- `hash(expression, xBits)` produces an array index from the expression and
  sample.
- `find(...)` searches for a matching entry and records a hit or miss.
- `insert(...)` stores a newly calculated value.
- `place(...)` inserts an entry into a table, probing another slot when there
  is a collision.
- `grow()` doubles the table and places its existing entries into the new
  table.
- `hits()` and `misses()` report cache statistics printed by the application.

The table uses open addressing with linear probing: if a slot is occupied by a
different key, it checks the next slot. It grows before becoming too full.

### `EquationHistory`: a linked list of plots

Each private `Node` stores one equation, its postfix form, the graph filename,
and a pointer to the next node.

- `add(...)` appends a new node to the tail.
- `print()` walks from the head to the end and displays every entry.
- `clear()` walks through and deletes every node.
- `size()` reports the number of entries.
- `~EquationHistory()` calls `clear()` to release the list's memory.

History exists only while GraphX is running; it is not saved to disk.

### `GraphSampler`: calculate values into an array

- `GraphSampler(minimum, maximum)` starts with the x and y viewing limits.
- `setRange(minimum, maximum)` checks and changes the limits.
- `sample(expression, cache, samples)` chooses 1201 evenly spaced x values,
  evaluates the expression for each x, and appends each result to the supplied
  `DynamicArray<GraphSample>`. It returns how many points could not be graphed.
- `minimum()` and `maximum()` let the renderer use the same viewing limits.

This is the simple graphing idea: calculate points first and store them in an
array. The renderer then draws those stored points, without doing math.

### SVG display code (`ui/SvgRenderer.cpp`)

- `writeSvg(expression, samples, minimum, maximum, fileName)` writes the SVG
  file from an existing sample array.
- `makeSvg(...)` creates SVG text for the background, grid, axes, labels, and
  curve.
- `escapeXml(text)` replaces XML-sensitive characters so the equation can be
  displayed safely.
- `mapX(...)` and `mapY(...)` convert math coordinates into image pixels.

SVG layout and styling are presentation code, not part of expression
calculation. This UI folder's presentation and console-interface code was
created with AI assistance and is kept separate from the course-focused core.
The output is plain text-based SVG and needs no graphics library.

## Useful C++ terms in this project

- **Class:** a reusable type that groups data and functions, such as
  `Expression`.
- **Template:** code parameterized by a type. `DynamicArray<T>` can store
  different kinds of items.
- **Pointer:** a variable that holds an address. Tree nodes and linked-list
  nodes use pointers to refer to other nodes.
- **Recursion:** a function calls itself on a smaller part of a structure.
  GraphX uses it to evaluate and destroy expression trees.
- **ADT (Abstract Data Type):** a type described by its operations rather than
  by how it is stored. `Stack` provides `push`, `pop`, and `top`.
- **Header/source split:** `.h` declares the interface, while `.cpp` implements
  the behavior.
- **`const`:** marks a value or method as not modifying the object through that
  interface.
- **Destructor:** a special function called when an object is destroyed; it
  releases resources such as dynamically allocated memory.

## Try it

Build with Ctrl+Shift+B in VS Code, then run `graphx.exe` from the project
folder. Try these commands:

```text
plot x^2
plot y = sin(x)
range -5 5
history
exit
```

Each successful plot replaces `graph.svg` in the current working directory.
Open that file in a browser to view the graph.
