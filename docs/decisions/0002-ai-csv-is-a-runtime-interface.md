# Treat AI CSV Weights As A Runtime Interface

## Status

Accepted existing constraint.

## Context

AI brains are loaded from CSV files at runtime. Brain enums and constructor assertions define their required input and output widths. Missing or unreadable files are fatal to AI initialization.

## Decision

Brain topology and enum order are a versioned interface between C++ and the CSV weights. A brain-interface change updates code, generated or trained weights, and focused tests together.

## Consequences

- Do not reorder existing input or output enum members without replacing compatible weights.
- Use `tools/gen_brain_csv.ps1` for structurally valid temporary weights; its output is not trained behavior.
- Validate a changed brain through its constructor dimensions and relevant behavior tests before relying on a full game run.
