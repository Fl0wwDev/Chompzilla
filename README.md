# README

## Description

Chompzilla is a "chomp" like game. It has a board with 7\*9 squares. It is a versus game with two player. Each player alternately break a square from the board. The player who break the last square loses the game.

## Rules of the game

1. The game is played on a 7x9 board.
2. Players take turns to break a square from the board.
3. When the player break a square, it also breaks all the squares to the right and below the square.
4. The player can not break more than five squares in a turn.
5. The player who break the last square will lose the game.

## How to build the game

1. Clone the repository with SVN.
    - `svn co https://svn.ensisa.uha.fr/scm/svn/imm2425-gr2`
2. Install the dependencies:
    - Run the script `install_deps.sh` to install the dependencies. (Works on Debian based systems and Fedora)
3. Run the command `make` to build the game with the tests and the documentation. You can also run the command `make game` to build only the game.
4. Run the command `./build/game` or `make run` to start the game.

## How to build the documentation

1. Run the command `make docs` to generate the documentation.
2. Open the file `docs/html/index.html` in a browser to view the documentation. 
   You can also use `make open_docs` to create and open the documentation.
3. You can also access to the documentation from [https://docs.ensisa.iutmulhouse.fr](https://docs.ensisa.iutmulhouse.fr).

## How to test the game

1. Run the command `make tests` to run the tests using criterion.
2. Run the command `make coverage` to generate the coverage report.
3. Open the file `tests/coverage/index.html` in a browser to view the coverage report.
   You can also use `make open_coverage` to create and open the coverage report.

## How to launch the game

1. Run the command `make run` to launch the game, or run the command `.build/game`.
2. You can add options to the command to play the game in different modes. Run the command `.build/game --help` to see the options.

## Game launch options

-   `-h` or `--help`: Display the help message.
-   `-g` or `--gui`: Launch the game with the graphical interface.
-   `-l` or `--local`: Launch the game in local mode. You can play with another player on the same computer.
-   `-s` or `--server`: Launch the game in server mode. You must add the port after the option like : `-s 5555`
-   `-ia` or `--ia`: To launch the game with an IA (in local mode, is it versus an IA, and in client you are the IA)
-   `-c` or `--client`: Launch the game in client mode. You must add the IP and the port of the server like : `-c 127.0.0.1:5555`


### Examples

-   To launch the game in local mode with the graphical interface: `./build/game -g -l`
-   To launch the game in CLI connecting to the 127.0.0.1 server on the port 5555: `./build/game -c 127.0.0.1:5555`
-   To launch the game in server mode on the port 5555: `./build/game -s 5555`
-   To launch the game in local mode with an IA: `./build/game -l -ia`
-   etc.

## How to play the game

You can play the game in different modes:

-   Player vs Player on the same computer.
-   Player vs IA.
-   Player vs Player on different computers with the help of a server.
-   IA vs other player on different computers with the help of a server.
-   IA vs IA on different computers with the help of a server.