UNAME := $(shell uname)

SRC_DIR = src
LIB_DIR = lib
TEST_DIR = test
INCLUDE_DIR = include

MAIN_BINARIES = $(wildcard $(SRC_DIR)/*.cc)
SOURCE_BINARIES = $(filter-out $(SRC_DIR)/main.cc, $(MAIN_BINARIES))
TEST_BINARIES = $(wildcard $(TEST_DIR)/*.cc)

## 3rd party libraries
TINYXML2_BINARIES = $(wildcard $(LIB_DIR)/tinyxml2/*.cpp)
# Includes
CATCH = -I ./$(LIB_DIR)/catch/
TINYXML2 = -I ./$(LIB_DIR)/tinyxml2/
LIBS = $(CATCH) $(TINYXML2)

## Compilation flags
# -g -> generate debug info
# -Werror -> treat every warning as an error
G++ = g++ -g -std=c++11 -Werror

ifeq ($(UNAME), Linux)
CLANG_FORMAT_BIN = clang-format-4.0
endif
ifeq ($(UNAME), Darwin)
CLANG_FORMAT_BIN = clang-format
endif

all: format build run

format:
	$(CLANG_FORMAT_BIN) -i $(SRC_DIR)/*.cc
	$(CLANG_FORMAT_BIN) -i $(INCLUDE_DIR)/*.h
	$(CLANG_FORMAT_BIN) -i $(TEST_DIR)/*.cc

clean:
	rm -rf *.out
	rm -rf *.dSYM/

build:
	$(G++) -o main.out $(MAIN_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(INCLUDE_DIR)

valgrind: build
	valgrind --leak-check=full ./main.out

.PHONY: test
test:
	$(G++) -o tests.out $(SOURCE_BINARIES) $(TEST_BINARIES) $(TINYXML2_BINARIES) $(LIBS) -I $(TEST_DIR) -I $(INCLUDE_DIR)
	./tests.out $(TEST_TAGS)

run:
	./main.out
