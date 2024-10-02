# 🦖 **Chompzilla**

### 📜 **Description**
Chompzilla is a "chomp"-like game. It is played on a **7x9** board. It is a **versus** game with **two players**. Each player alternately breaks a square from the board. **The player who breaks the last square loses the game.**

### 🎮 **Game Rules**
- The game is played on a **7x9** board.
- Players take turns to break a square from the board.
- When a player breaks a square, **all the squares to the right and below** that square are also broken.
- A player **cannot break more than five squares** in a single turn.
- The player who breaks the last square **loses** the game.

### 🏗️ **How to Build the Game**
1. Clone the repository using SVN:
    ```bash
    svn co https://svn.ensisa.uha.fr/scm/svn/imm2425-gr2
    ```
2. Install the dependencies:
    ```bash
    ./install_deps.sh
    ```
   (Works on Debian-based and Fedora systems)
3. Build the game along with tests and documentation:
    ```bash
    make
    ```
   To build only the game:
    ```bash
    make game
    ```
4. Run the game:
    ```bash
    ./build/game
    ```
    Or use the command:
    ```bash
    make run
    ```

### 📚 **How to Build the Documentation**
1. Generate the documentation:
    ```bash
    make docs
    ```
2. Open the file `docs/html/index.html` in a browser to view the documentation, or use:
    ```bash
    make open_docs
    ```
3. The documentation is also accessible at: [Documentation](https://docs.ensisa.iutmulhouse.fr).

### ✅ **How to Test the Game**
1. Run the tests using Criterion:
    ```bash
    make tests
    ```
2. Generate the coverage report:
    ```bash
    make coverage
    ```
3. Open the file `tests/coverage/index.html` in a browser, or use:
    ```bash
    make open_coverage
    ```

### 🚀 **Game Launch Options**
- `-h` or `--help`: Display the help message.
- `-g` or `--gui`: Launch the game with the graphical interface.
- `-l` or `--local`: Launch the game in local mode. You can play with another player on the same computer.
- `-s` or `--server`: Launch the game in server mode. You must specify the port after the option, like: `-s 5555`.
- `-ia` or `--ia`: Launch the game with an AI (In local mode, it is versus an AI; in client mode, you are the AI).
- `-c` or `--client`: Launch the game in client mode. You must add the IP and the server port, like: `-c 127.0.0.1:5555`.

### 🎮 **Launch Examples**
- Launch the game in local mode with the graphical interface:
    ```bash
    ./build/game -g -l
    ```
- Launch the game in CLI mode connecting to the server `127.0.0.1` on port `5555`:
    ```bash
    ./build/game -c 127.0.0.1:5555
    ```
- Launch the game in server mode on port `5555`:
    ```bash
    ./build/game -s 5555
    ```
- Launch the game in local mode against an AI:
    ```bash
    ./build/game -l -ia
    ```

### 💻 **Game Modes**
- Player vs. Player on the same computer.
- Player vs. AI.
- Player vs. Player on different computers using a server.
- AI vs. Player on different computers using a server.
- AI vs. AI on different computers using a server.

### 👥 **Contributors**
- **MathiasLinux** 🤝
