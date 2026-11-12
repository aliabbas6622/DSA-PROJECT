# GraphX UI Layer

This folder contains the presentation and command-line interaction code. It is
kept separate from the course-focused core data structures and math engine.

The SVG styling, coordinate layout, and console interaction in this folder were
created with AI assistance. The code is kept here so its role and authorship
are easy to identify and discuss.

## Files

- `ConsoleApp.h` declares `graphx::ui::run()`, the entry point for the
  interactive command-line interface.
- `ConsoleApp.cpp` prints prompts/help, reads commands, manages the current
  session, asks the core to parse and sample equations, and passes the samples
  to the SVG writer.
- `SvgRenderer.h` declares the function that writes an SVG graph.
- `SvgRenderer.cpp` turns already-calculated `(x, y)` samples into an SVG
  grid, axes, labels, and curve. It does not evaluate equations.

The console UI asks the core `GraphSampler` to fill a dynamic array of sample
values, then passes that array to the SVG writer. This separation means the
math and DSA code can be understood or tested without needing to understand
SVG markup.
