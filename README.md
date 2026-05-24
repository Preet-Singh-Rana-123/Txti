# Txti

A lightweight terminal-based text editor built from scratch in C++ using the ncurses library.

> 🚧 **Active Development** — Txti is growing fast. Core editing features are live and more are on the way.

---

## Current Features

- **File argument support** — open any file directly from the command line (`./TextEditor file.txt`)
- **Line numbers** — sidebar showing line count for every line
- **Cursor navigation** — move freely with arrow keys (↑ ↓ ← →)
- **Header bar** — displays the current filename centered at the top
- **Footer bar** — always-visible keybinding reference at the bottom
- **Save** — write changes to disk with `Ctrl+A`
- **Quit** — exit the editor cleanly with `Ctrl+X`

---

## Planned Features

- [ ] Delete current line
- [ ] Delete word (forward/backward)
- [ ] Copy and Paste feature
- [ ] Udo and Redo Operations
- [ ] Configuration menu for UI customization
- [ ] Integrated file system browser
- [ ] Multiple file tabs / buffers
- [ ] LSP integration for code editing (autocomplete, diagnostics, go-to-definition)

---

## Prerequisites

Make sure the following are installed on your system:

- A C++17-compatible compiler (e.g. `g++`, `clang++`)
- [CMake](https://cmake.org/) ≥ 3.19
- ncurses development library

**On Ubuntu/Debian:**
```bash
sudo apt install build-essential cmake libncurses-dev
```

**On Arch Linux:**
```bash
sudo pacman -S base-devel cmake ncurses
```

**On macOS (Homebrew):**
```bash
brew install cmake ncurses
```

---

## Building

Clone the repository and build using CMake (out-of-source build recommended):

```bash
git clone https://github.com/Preet-Singh-Rana-123/Txti.git
cd Txti
mkdir build && cd build
cmake ..
make
```

The compiled binary will be at `build/TextEditor`.

---

## Usage

```bash
./TextEditor <filename>
```

**Example:**
```bash
./TextEditor notes.txt
```

### Keybindings

| Key | Action |
|-----|--------|
| `↑` `↓` `←` `→` | Move cursor |
| `Ctrl+A` | Save file |
| `Ctrl+X` | Quit |

---

## Project Structure

```
Txti/
├── include/           # Header files
├── src/
│   ├── main.cpp       # Entry point
│   └── textEditor.cpp # Core editor logic
├── CMakeLists.txt     # Build configuration
└── .gitignore
```

---

## Tech Stack

- **Language:** C++17
- **TUI Library:** [ncurses](https://invisible-island.net/ncurses/)
- **Build System:** CMake

---

## Contributing

This is a personal learning project, but feedback and suggestions are welcome! Feel free to open an issue or fork the repo.

---

## License

This project does not currently have a license. All rights reserved by the author.

---

*Built with curiosity and C++ by [Preet Singh Rana](https://github.com/Preet-Singh-Rana-123)*
