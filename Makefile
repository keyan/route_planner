UNAME := $(shell uname)

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

ifeq ($(UNAME), Linux)
CLANG_FORMAT_BIN = clang-format-4.0
endif
ifeq ($(UNAME), Darwin)
CLANG_FORMAT_BIN = clang-format
endif

all: format build run

format:
	$(CLANG_FORMAT_BIN) -i $(SRC_DIR)/*.cc
	$(CLANG_FORMAT_BIN) -i $(SRC_DIR)/*.h

clean:
	rm -rf *.out
	rm -rf *.dSYM/

# -g -> generate debug info
# -Werror -> treat every warning as an error
build:
	g++ -g -std=c++11 -o main.out -Werror $(MAIN_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(SRC_DIR)

valgrind: build
	valgrind --leak-check=full ./main.out

.PHONY: test
test:
	g++ -std=c++11 -o tests.out -Werror $(SOURCE_BINARIES) $(TEST_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(SRC_DIR)
	./tests.out

test_debug:
	g++ -g -std=c++11 -o tests.out -Werror $(TEST_BINARIES) $(LIBS) -I ./src/*.cc
	gdb ./tests.out

run:
	./main.out
