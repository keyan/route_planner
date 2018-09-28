SRC_DIR = src
LIB_DIR = lib
TEST_DIR = test

MAIN_BINARIES = $(wildcard $(SRC_DIR)/*.cc)
TEST_BINARIES = $(wildcard $(TEST_DIR)/*.cc)

# HEADERS = $(wildcard $(SRC_DIR)/*.h)

# 3rd party libraries
CATCH = -I ./$(LIB_DIR)/catch/
TINYXML2 = -I ./$(LIB_DIR)/tinyxml2/
LIBS = $(CATCH) $(TINYXML2)

.PHONY: all
all: format build run

.PHONY: format
format:
	clang-format -i $(SRC_DIR)/*.cc
	clang-format -i $(SRC_DIR)/*.h

.PHONY: clean
clean:
	rm -rf *.out

build:
	# -Werror -> treat every warning as an error
	g++ -std=c++11 -o main.out -Werror $(MAIN_BINARIES) ./lib/tinyxml2/tinyxml2.cpp $(LIBS) -I .

.PHONY: test
test:
	g++ -std=c++11 -o tests.out -Werror $(TEST_BINARIES) $(LIBS) -I ./src/*.cc
	./tests.out

.PHONY: test_debug
test_debug:
	g++ -g -std=c++11 -o tests.out -Werror $(TEST_BINARIES) $(LIBS) -I ./src/*.cc
	gdb ./tests.out

.PHONY: run
run:
	./main.out
