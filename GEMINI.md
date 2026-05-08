# Project Overview: TOC Final Project - NFA/DFA Converter & Simulator

This project is a C++ application designed for the **Theory of Computation (TOC)** final project. It provides tools for defining, converting, and simulating Finite Automata, specifically Non-deterministic Finite Automata (NFA) and Deterministic Finite Automata (DFA).

## Core Features
- **NFA to DFA Conversion:** Implements the subset construction algorithm to convert a given NFA into its equivalent DFA.
- **Automata Simulation:** Allows users to test input strings against the generated DFA or NFA to verify acceptance or rejection.
- **Graphical Visualization:** Generates DOT files and PDFs (requires Graphviz) to visualize the states and transitions of the automata.
- **DFA to NFA Conversion:** Supports converting a DFA back into an NFA representation.

## Architecture
The project follows a modular C++ structure:
- `main.cpp`: Entry point containing the interactive menu-driven interface.
- `Headers/`: Contains header files defining the classes and interfaces.
- `Classes/`: Contains the implementation of the logic defined in the headers.
  - `NFA.cpp` / `DFA.cpp`: Data structures for the automata.
  - `AutomataConverter.cpp`: Logic for NFA ↔ DFA conversion.
  - `AutomataSimulator.cpp`: Logic for processing strings through the automata.
  - `AutomataVisualizer.cpp`: Utilities for generating DOT and PDF visualizations.
  - `UserInputHandler.cpp`: Handles interactive console input for defining automata.
  - `Utils.cpp`: Common utility functions (e.g., input validation).

## Technologies
- **Language:** C++17
- **Compiler:** `g++`
- **Build System:** `make`
- **External Dependencies:** `Graphviz` (optional, for `dot` to `pdf` conversion).

---

## Building and Running

### Prerequisites
Ensure you have `g++` and `make` installed. For visualization, `graphviz` should be available in your system path.

### Build
To compile the project and generate the `FiniteAutomata` executable:
```bash
make
```

### Run
To run the interactive simulator:
```bash
./FiniteAutomata
```
Alternatively, you can use the provided helper script:
```bash
./run.sh
```

### Clean
To remove object files, build artifacts, and generated PDF/DOT files:
```bash
make clean
```
Or:
```bash
./clean.sh
```

---

## Development Conventions
- **Code Style:** Uses modular class-based design with separation of concerns between logic and visualization.
- **Error Handling:** Employs custom input validation (see `Utils.cpp` and `UserInputHandler.cpp`) to handle malformed user input in the CLI.
- **Visualization:** Automata are visualized using the DOT language. The `AutomataVisualizer` class handles the conversion from internal representation to DOT format.
