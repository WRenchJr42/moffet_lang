# MOFFET Language

Moffet is an **experimental LLVM-based language frontend** built to explore compiler design, program analysis, and LLVM IR generation.  
The project focuses on understanding how source code is transformed through lexical analysis, parsing, AST construction, and lowered into LLVM Intermediate Representation (IR).

This project is educational in nature and was created to gain hands-on experience with **compiler frontends and low-level program representation**, forming a foundation for future work in static analysis and security tooling.

---

## Overview

Moffet currently implements:
- A **lexer** for tokenizing input
- A **recursive-descent parser**
- An **AST (Abstract Syntax Tree)** representation
- **Operator precedence parsing**
- **Basic LLVM IR code generation** using `IRBuilder`

The design is inspired by LLVM’s *Kaleidoscope* tutorial, with custom modifications and extensions to deepen understanding of compiler pipelines.

---

## Implemented Features

- Lexical analysis for identifiers, numbers, operators, and keywords
- Parsing of:
  - Numeric expressions
  - Variable references
  - Binary operations
  - Function prototypes and calls
- AST-based semantic representation
- Basic code generation for supported expressions
- Interactive REPL-style execution loop

---

## Current Status & Limitations

This implementation is **experimental and not production-ready**:
- Error handling is minimal
- Only a limited subset of language features is supported
- Some components are still under refinement

These limitations are intentional, as the primary goal of the project is **learning and correctness**, not completeness.

---

## Motivation

This project was built to:
- Understand compiler frontend architecture
- Gain familiarity with LLVM internals and IR
- Explore how program structure can be modeled for future **static analysis** and **security-oriented tooling**

---

## Run Locally

Clone the project:

```bash
git clone https://github.com/WRenchJr42/moffet_lang.git
