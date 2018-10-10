SRC_DIR = src
LIB_DIR = lib
TEST_DIR = test

MAIN_BINARIES = $(wildcard $(SRC_DIR)/*.cc)
SOURCE_BINARIES = $(filter-out $(SRC_DIR)/main.cc, $(MAIN_BINARIES))
TEST_BINARIES = $(wildcard $(TEST_DIR)/*.cc)

## 3rd party libraries
TINYXML2_BINARIES = $(wildcard $(LIB_DIR)/tinyxml2/*.cpp)
# Includes
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
	g++ -g -std=c++11 -o main.out -Werror $(MAIN_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(SRC_DIR)

.PHONY: test
test:
	g++ -std=c++11 -o tests.out -Werror $(SOURCE_BINARIES) $(TEST_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(SRC_DIR)
	./tests.out

.PHONY: test_debug
test_debug:
	g++ -g -std=c++11 -o tests.out -Werror $(TEST_BINARIES) $(LIBS) -I ./src/*.cc
	gdb ./tests.out

.PHONY: run
run:
	./main.out
