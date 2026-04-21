# 🖥️ Smart CLI

A lightweight, intelligent command-line interpreter built from scratch in **C++** — featuring real-time command prediction using a **Trie**, frequency-based ranking using a **Hash Map**, command history navigation with arrow keys, and support for 11 Unix-style commands. Runs natively on **Windows, Linux, and macOS**.

---

## ✨ Features

- 🔮 **Smart Prediction** — As you type, the CLI suggests the most frequently used matching command in real-time
- ⌨️ **TAB to Complete** — Press `Tab` to instantly fill in the suggested command
- 🔁 **History Navigation** — Press `↑` / `↓` arrow keys to scroll through previously entered commands
- 💾 **Persistent History** — Command frequency is saved to `.smart_cli_history` and reloaded on every launch, so predictions improve over time
- 🎨 **Colored Output** — ANSI color support with auto-enable on Windows
- 🪟 **Cross-Platform** — Works on Windows (MinGW/MSVC), Linux, and macOS

---

## 📦 Supported Commands

| Command | Syntax | Description |
|---|---|---|
| `echo` | `echo <text>` | Print text to the screen |
| `cat` | `cat <file>` | Display file contents |
| `touch` | `touch <file>` | Create one or more files |
| `head` | `head [-n N] <file>` | Print first N lines (default: 10) |
| `tail` | `tail [-n N] <file>` | Print last N lines (default: 10) |
| `nl` | `nl <file>` | Print file with numbered non-empty lines |
| `wc` | `wc [-l\|-w\|-c] <file>` | Count lines, words, or characters |
| `bc` | `bc <expression>` | Evaluate math expressions with `+ - * / ()` |
| `expr` | `expr <a> <op> <b>` | Simple two-operand arithmetic |
| `factor` | `factor <n>` | Prime factorization of a number |
| `grep` | `grep [-i] [-n] [-v] <pattern> <file>` | Search for a pattern in a file |
| `help` | `help` | List all available commands |
| `exit` | `exit` | Quit the CLI |

---

## 🧠 How the Smart Prediction Works

The prediction engine is built on two core data structures:

**Trie (Prefix Tree)**
- Every command you run is inserted into a Trie character by character
- When you start typing, the Trie traverses to your current prefix and collects all matching completions

**Hash Map (Frequency Counter)**
- A `freqMap` tracks how many times each command has been used
- When multiple completions exist, the one with the highest frequency is suggested first

This means the CLI learns your habits — the more you use a command, the higher it ranks in suggestions.

---

## 🚀 Getting Started

### Prerequisites
- A C++17 compatible compiler:
  - **Windows**: [MinGW-w64](https://www.mingw-w64.org/) or MSVC (Visual Studio)
  - **Linux/macOS**: GCC or Clang (usually pre-installed)

### Build & Run

**Windows (MinGW)**
```bash
g++ -std=c++17 -o smart_cli smart_CLI.cpp
smart_cli.exe
```

**Linux / macOS**
```bash
g++ -std=c++17 -o smart_cli smart_CLI.cpp
./smart_cli
```

**Visual Studio (Windows)**
1. Open Visual Studio → Create new Empty C++ Project
2. Add `smart_CLI.cpp` to the project
3. Set C++ Standard to C++17 in Project Properties
4. Build and Run (`Ctrl+F5`)

---

## 🎮 Usage Examples

```
>echo "Hello World"
Hello World

>head -n 5 myfile.txt
(prints first 5 lines)

>tail -n 3 myfile.txt
(prints last 3 lines)

>bc (3+5)*2
16

>expr 10 % 3
1

>factor 360
360: 2 2 2 3 3 5

>grep -i -n hello myfile.txt
3:Hello World
7:hello there
```

---

## 📁 Project Structure

```
Smart CLI/
└── smart_CLI.cpp       # Single-file implementation
```

---

## 🛠️ Technologies Used

- **Language**: C++17
- **Data Structures**: Trie, Hash Map (`unordered_map`), Stack, Vector
- **Platform APIs**: `conio.h` (Windows) / `termios.h` (Linux/macOS)
- **Concepts**: File I/O, Raw terminal input, ANSI escape codes, Operator precedence parsing

---

## 🔮 Future Plans

- [ ] `ls` — list directory contents
- [ ] `mkdir` / `rm` — create and delete directories
- [ ] `pwd` — print working directory
- [ ] `|` pipe operator support between commands
- [ ] `>` / `>>` output redirection
- [ ] Multi-word history search with `Ctrl+R`

---

## 👨‍💻 Author

**Krish Mojidra**  
B.Tech Computer Science Engineering | Nirma University  
📧 krishmojidra3010@gmail.com  

