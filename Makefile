# Declaration of the constants
CC = gcc
SRC_DIR = src
BUILD_DIR = build
SRC_FILES = $(wildcard ${SRC_DIR}/*/*.c) $(wildcard ${SRC_DIR}/*.c)
# Exclude the test files from the build (e.g. src/tests/game_logic.test.c)
EXCLUDE = $(filter-out ${SRC_DIR}/tests/%, ${SRC_FILES})
TEST_BUILD_DIR = tests
TEST_SRC_FILES = $(wildcard ${SRC_DIR}/test/*.c)
EXCLUDE_MAIN = $(filter-out ${SRC_DIR}/game.c, ${SRC_FILES})
COVERAGE_DIR = $(TEST_BUILD_DIR)/coverage
COVERAGE_INFO_FILE = $(COVERAGE_DIR)/coverage.info
DOCS_DIR = docs
DOXYFILE = $(DOCS_DIR)/Doxyfile
CHOMPZILLA_LOGO = $(DOCS_DIR)/chompzilla_logo.png
CFLAGS = $(shell pkg-config --cflags gtk4)
CLIBS = $(shell pkg-config --libs gtk4)

# Main target (all)
all: $(BUILD_DIR) tests coverage docs

# Target: to compile the test files
tests: create_build_dir create_test_dir
	$(CC) -o $(TEST_BUILD_DIR)/test $(TEST_SRC_FILES) $(EXCLUDE_MAIN) $(CFLAGS) -fprofile-arcs -ftest-coverage $(CLIBS) -lcriterion -lgcov
	$(TEST_BUILD_DIR)/test

# Target: to create the coverage report
coverage: tests create_coverage_dir
# We verify if lcov can use the mismatch option
	[ "$(shell lcov --help | grep mismatch)" = "" ] && lcov --capture --directory $(TEST_BUILD_DIR) --output-file $(COVERAGE_INFO_FILE) || lcov --capture --directory $(TEST_BUILD_DIR) --output-file $(COVERAGE_INFO_FILE) --ignore-errors mismatch
	genhtml $(COVERAGE_INFO_FILE) --output-directory $(COVERAGE_DIR)
	echo "Coverage report generated at $(COVERAGE_DIR)/index.html"

# Target: to open the coverage report
open_coverage: coverage
	if [ "$(shell uname)" = "Linux" ]; then xdg-open $(COVERAGE_DIR)/index.html; fi
	if [ "$(shell uname)" = "Darwin" ]; then open $(COVERAGE_DIR)/index.html; fi

# Target: to generate the documentation
docs: create_docs_dir
	doxygen $(DOXYFILE)
	echo "Documentation generated at $(DOCS_DIR)/html/index.html"

# Target: to open the documentation
open_docs: docs
	if [ "$(shell uname)" = "Linux" ]; then xdg-open $(DOCS_DIR)/html/index.html; fi
	if [ "$(shell uname)" = "Darwin" ]; then open $(DOCS_DIR)/html/index.html; fi

# Target: build the game
game: $(BUILD_DIR)

# Target: to run the game
run: $(BUILD_DIR)
	$(BUILD_DIR)/game

# Target: to clean the folder
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(TEST_BUILD_DIR)/*
# Remove all the docs except the Doxyfile
# Get all the docs file to remove
	rm -rf $(filter-out $(DOXYFILE) $(CHOMPZILLA_LOGO), $(wildcard $(DOCS_DIR)/*))

# Target: to create the build directory if it does not exist
create_build_dir:
	mkdir -p $(BUILD_DIR)

# Target: to create the test directory if it does not exist
create_test_dir:
	mkdir -p $(TEST_BUILD_DIR)

# Target: to create the coverage directory
create_coverage_dir: create_test_dir
	mkdir -p $(COVERAGE_DIR)

# Target: to create the documentation directory
create_docs_dir:
	mkdir -p $(DOCS_DIR)

# Target: to transform the c files in object files without the test files
$(BUILD_DIR): create_build_dir
# We create all the object files
	$(CC) -c $(EXCLUDE) $(CFLAGS) $(CLIBS)
# We move the object files to the build directory
	mv *.o $(BUILD_DIR)
# We compile the object files in an executable
	$(CC) -o $(BUILD_DIR)/game $(BUILD_DIR)/*.o $(CFLAGS) $(CLIBS)
