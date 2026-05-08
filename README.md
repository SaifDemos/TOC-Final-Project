# TOC Final Project — Documentation
**Theory of Computation | NFA ↔ DFA Converter & Simulator**
*Language: C++ | Author: SaifDemos*

---

## Table of Contents
1. [What Is This Project?](#1-what-is-this-project)
2. [Key Concepts (Quick Glossary)](#2-key-concepts-quick-glossary)
3. [Project Structure](#3-project-structure)
4. [File-by-File Breakdown](#4-file-by-file-breakdown)
   - [main.cpp](#maincpp)
   - [Headers/NFA.h](#headersnfah)
   - [Headers/DFA.h](#headersdfah)
   - [Headers/AutomataConverter.h](#headersautomataconverterh)
   - [Headers/AutomataSimulator.h](#headersautomatasimulatorh)
   - [Headers/AutomataVisualizer.h](#headersautomatavisualizerh)
   - [Headers/UserInputHandler.h](#headersuserinputhandlerh)
   - [Headers/Utils.h](#headersutilsh)
   - [Classes/ folder](#classes-folder)
   - [.gitignore](#gitignore)
5. [How the Program Works — Step by Step](#5-how-the-program-works--step-by-step)
6. [The Core Algorithm: NFA → DFA (Subset Construction)](#6-the-core-algorithm-nfa--dfa-subset-construction)
7. [The Reverse: DFA → NFA](#7-the-reverse-dfa--nfa)
8. [Data Flow Diagram](#8-data-flow-diagram)
9. [How to Explain This to a Friend](#9-how-to-explain-this-to-a-friend)

---

## 1. What Is This Project?

This is a **C++ console application** built as a final project for a Theory of Computation (TOC) course. Its two main capabilities are:

- **NFA → DFA**: Take a Non-deterministic Finite Automaton defined by the user, convert it into an equivalent Deterministic Finite Automaton using the *Subset Construction* algorithm, visualize both, and then let the user test strings against the resulting DFA.
- **DFA → NFA**: Take a user-defined DFA, convert it back into an NFA (a straightforward structural conversion), visualize it, and let the user test strings against it.

The program also generates **PDF diagrams** of both automata so you can see the state graphs visually.

---

## 2. Key Concepts (Quick Glossary)

| Term | Plain English |
|---|---|
| **Finite Automaton (FA)** | A machine that reads a string symbol-by-symbol and ends up in a state that is either "accepting" (yes) or "rejecting" (no). |
| **State** | A configuration the machine can be in. Think of it like a node in a flowchart. |
| **Alphabet (Σ)** | The set of allowed input characters (e.g., `{a, b}`). |
| **Transition** | A rule: "if I'm in state X and I read symbol Y, go to state Z." |
| **Start State (q0)** | Where the machine begins. |
| **Accept / Final State** | If the machine ends here after reading the whole string, the string is **accepted**. |
| **NFA** | Allows *multiple possible next states* for one symbol (or even ε-transitions — moving without reading anything). |
| **DFA** | Exactly *one* next state for every (state, symbol) pair. No ambiguity. |
| **Subset Construction** | The classic algorithm to convert any NFA into an equivalent DFA. |
| **ε-closure** | From a given state, all states reachable by following ε (empty) transitions. |

---

## 3. Project Structure

```
TOC-Final-Project/
│
├── main.cpp                    ← Entry point; the main menu loop
│
├── Headers/                    ← Declarations (.h files)
│   ├── NFA.h
│   ├── DFA.h
│   ├── AutomataConverter.h
│   ├── AutomataSimulator.h
│   ├── AutomataVisualizer.h
│   ├── UserInputHandler.h
│   └── Utils.h
│
├── Classes/                    ← Implementations (.cpp files matching each header)
│   ├── NFA.cpp
│   ├── DFA.cpp
│   ├── AutomataConverter.cpp
│   ├── AutomataSimulator.cpp
│   ├── AutomataVisualizer.cpp
│   ├── UserInputHandler.cpp
│   └── Utils.cpp
│
└── .gitignore                  ← Tells Git which files to ignore
```

The project follows the classic C++ pattern: **headers declare** what exists, **classes implement** how it works.

---

## 4. File-by-File Breakdown

---

### `main.cpp`

**Role:** The entry point and the brain of the user interface.

**What it does:**
- Prints the welcome banner.
- Holds the main `do-while` loop that keeps showing the menu until the user exits.
- Reads the user's menu choice (1, 2, or 0) using `readInt()` from Utils.
- For **Choice 1 (NFA → DFA)**:
  1. Checks if an NFA is already stored. If so, asks whether to reuse it or input a new one.
  2. Calls `UserInputHandler::inputCustomNFA()` to collect NFA data from the user.
  3. Calls `AutomataConverter::nfaToDfa()` to convert.
  4. Calls `AutomataVisualizer::drawNFAGraphical()` and `drawDFAGraphical()` to display both.
  5. Saves both as PDFs via `AutomataVisualizer::saveNfaToPdf()` and `saveDfaToPdf()`.
  6. Enters a string-testing loop: reads strings one at a time and calls `AutomataSimulator::simulateDfa()`.
- For **Choice 2 (DFA → NFA)**:
  1. Same pattern — asks to reuse or re-enter a DFA.
  2. Calls `UserInputHandler::inputCustomDFA()`.
  3. Calls `AutomataConverter::dfaToNfa()`.
  4. Visualizes and saves the NFA PDF.
  5. Enters a string-testing loop calling `AutomataSimulator::simulateNfa()`.

**Key variables:**
- `currentNFA` — stores the most recently used NFA.
- `currentDFA` — stores the most recently used DFA.
- `choice` — the menu selection.

---

### `Headers/NFA.h`

**Role:** Defines the **NFA data structure**.

**What it declares:**
- A `struct` (or `class`) called `NFA` that holds:
  - `numStates` — how many states the NFA has.
  - `alphabet` — a set of characters (e.g., `{'a', 'b'}`).
  - `startState` — the initial state index.
  - `acceptStates` — the set of accepting/final states.
  - `transitions` — a map from `(state, symbol)` to a *set* of possible next states (because NFA transitions are non-deterministic — one input can lead to many states).
  - ε-transitions are typically stored as transitions on a special empty symbol (often `'\0'` or `'e'`).

**Why it's important:** Every other module depends on this type. Without the NFA definition, nothing else compiles.

---

### `Headers/DFA.h`

**Role:** Defines the **DFA data structure**.

**What it declares:**
- A `struct` (or `class`) called `DFA` that holds:
  - `numStates` — how many states.
  - `alphabet` — the same set of characters.
  - `startState` — initial state.
  - `acceptStates` — set of accepting states.
  - `transitions` — a map from `(state, symbol)` to exactly **one** next state (deterministic!).

**Key difference from NFA:** The transition function returns a single state, not a set.

---

### `Headers/AutomataConverter.h`

**Role:** Declares the conversion functions.

**What it declares:**
- `static DFA nfaToDfa(const NFA& nfa)` — converts an NFA to an equivalent DFA using the Subset Construction algorithm.
- `static NFA dfaToNfa(const DFA& dfa)` — converts a DFA back into an NFA (every DFA is technically already an NFA where each transition has exactly one destination).

This is the **mathematical heart** of the project. See Section 6 for how the algorithm works.

---

### `Headers/AutomataSimulator.h`

**Role:** Declares functions for running automata on input strings.

**What it declares:**
- `static void simulateDfa(const DFA& dfa, const string& input)` — runs a string through a DFA step-by-step and prints whether it is **accepted** or **rejected**. This is deterministic: at each step there is exactly one path.
- `static void simulateNfa(const NFA& nfa, const string& input)` — runs a string through an NFA. Because NFAs can branch, this uses a set of "current states" that grows/shrinks as ε-closures and transitions are applied. If *any* branch ends in an accept state, the string is accepted.

---

### `Headers/AutomataVisualizer.h`

**Role:** Declares the diagram-drawing and PDF-saving functions.

**What it declares:**
- `static void drawNFAGraphical(const NFA& nfa)` — prints an ASCII/graphical representation of the NFA in the terminal.
- `static void drawDFAGraphical(const DFA& dfa)` — same for the DFA.
- `static void saveNfaToPdf(const NFA& nfa, const string& filename)` — generates a PDF file with the NFA's state diagram.
- `static void saveDfaToPdf(const DFA& dfa, const string& filename)` — generates a PDF for the DFA.

This is the "output" layer — it makes the abstract data structures visible and understandable.

---

### `Headers/UserInputHandler.h`

**Role:** Declares functions for collecting automaton definitions from the user via the terminal.

**What it declares:**
- `static NFA inputCustomNFA()` — walks the user through entering: the number of states, the alphabet, the start state, the accept states, and all transitions (including ε-transitions). Returns a filled-out `NFA` object.
- `static DFA inputCustomDFA()` — same for a DFA, but enforces that each `(state, symbol)` pair has exactly one destination.

This is the "input" layer — it turns user keystrokes into structured data.

---

### `Headers/Utils.h`

**Role:** Declares small utility/helper functions used throughout the project.

**What it declares:**
- `bool readInt(int& value, int min, int max, const string& errorMsg)` — safely reads an integer from the user within a valid range. Returns `false` if the input was invalid, and prints an error message. Used in `main.cpp` for menu and sub-choice input.
- `string trim(const string& s)` — removes leading and trailing whitespace from a string. Used to clean up input strings before testing them against an automaton.
- `void waitForUser()` — pauses the program and waits for the user to press Enter before continuing. Used between visualizer outputs so the user can read them.

---

### `Classes/` folder

Each `.cpp` file in this folder **implements** the corresponding header. For example:
- `Classes/AutomataConverter.cpp` contains the actual C++ code for `nfaToDfa()` and `dfaToNfa()`.
- `Classes/AutomataSimulator.cpp` contains the simulation logic.
- etc.

This separation (declaration in `Headers/`, definition in `Classes/`) is standard C++ practice and makes the code easier to navigate.

---

### `.gitignore`

**Role:** Tells Git (the version-control system) which files to *not* track.

Typically contains things like compiled binaries (`*.exe`, `*.o`), generated PDF output files, and IDE/editor configuration folders. It keeps the repository clean — only source code is committed, not build artifacts.

---

## 5. How the Program Works — Step by Step

Here is the full user journey for the NFA → DFA path:

```
Program starts
     │
     ▼
 Main menu displayed
     │
     ├─ User picks 1 (NFA → DFA)
     │        │
     │        ▼
     │   [UserInputHandler] Prompts for:
     │     - Number of states
     │     - Alphabet symbols
     │     - Start state
     │     - Accept states
     │     - Transition table (including ε)
     │        │
     │        ▼
     │   NFA object created in memory
     │        │
     │        ▼
     │   [AutomataConverter::nfaToDfa()] runs Subset Construction
     │        │
     │        ▼
     │   DFA object created in memory
     │        │
     │        ▼
     │   [AutomataVisualizer] draws NFA in terminal + saves NFA PDF
     │   [AutomataVisualizer] draws DFA in terminal + saves DFA PDF
     │        │
     │        ▼
     │   String test loop:
     │     - User types a string
     │     - [Utils::trim()] cleans it
     │     - Validated against the DFA's alphabet
     │     - [AutomataSimulator::simulateDfa()] runs it
     │     - "ACCEPTED" or "REJECTED" printed
     │     - Repeat until user types "exit"
     │
     └─ User picks 0 → Program exits
```

---

## 6. The Core Algorithm: NFA → DFA (Subset Construction)

This is the theoretical centerpiece of the project. Here is how it works conceptually:

**The big idea:** A DFA state corresponds to a *set* of NFA states. We track all the NFA states the machine *could* be in simultaneously.

**Step-by-step:**

1. **Compute the ε-closure of the NFA's start state.** This gives you the initial DFA state (a set of NFA states reachable without reading anything).

2. **For each unprocessed DFA state** (which is a set of NFA states) and **for each symbol in the alphabet:**
   - Find all NFA states reachable from any state in the current set by reading that symbol.
   - Compute the ε-closure of those reachable states.
   - That resulting set of NFA states becomes a new DFA state.

3. **Repeat** until no new DFA states are discovered.

4. **Mark accept states:** any DFA state that contains at least one NFA accept state is itself an accept state.

**Example with 3 NFA states `{q0, q1, q2}`:**

| DFA State (NFA subset) | on 'a' → | on 'b' → |
|---|---|---|
| {q0} | {q0, q1} | {q0} |
| {q0, q1} | {q0, q1} | {q0, q2} |
| {q0, q2} | {q0, q1} | {q0} ← q2 is accept, so this DFA state accepts |

The DFA may have up to **2^n** states (where n is the number of NFA states), but in practice many subsets are unreachable and are never created.

---

## 7. The Reverse: DFA → NFA

This is much simpler. Since every DFA is already a valid NFA (just one where each `(state, symbol)` transition happens to go to exactly one state), the conversion is mostly a structural repackaging:

- Copy all states, alphabet, start state, and accept states as-is.
- Rewrite each DFA transition `δ(q, a) = p` as an NFA transition `δ(q, a) = {p}` (a set containing just one state).
- No ε-transitions are needed.

The resulting NFA is logically identical to the DFA — it just lives in the NFA data structure.

---

## 8. Data Flow Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                         main.cpp                             │
│                     (Menu + Orchestration)                   │
└──────┬──────────────────────────────────────────┬────────────┘
       │                                          │
       ▼                                          ▼
┌─────────────────┐                    ┌─────────────────────┐
│ UserInputHandler│                    │       Utils         │
│                 │                    │  readInt / trim /   │
│ inputCustomNFA()│                    │  waitForUser        │
│ inputCustomDFA()│                    └─────────────────────┘
└────────┬────────┘
         │ produces NFA / DFA objects
         ▼
┌───────────────────────┐
│  AutomataConverter    │
│                       │
│  nfaToDfa(NFA) → DFA  │
│  dfaToNfa(DFA) → NFA  │
└──────────┬────────────┘
           │ produces converted automaton
     ┌─────┴──────┐
     ▼            ▼
┌──────────┐  ┌──────────────────┐
│Simulator │  │  Visualizer      │
│          │  │                  │
│simulateDfa│ │ drawNFAGraphical │
│simulateNfa│ │ drawDFAGraphical │
│ → ACCEPT │  │ saveNfaToPdf     │
│ / REJECT │  │ saveDfaToPdf     │
└──────────┘  └──────────────────┘
```

---

