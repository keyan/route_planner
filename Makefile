UNAME := $(shell uname)

BOOST_VERSION = 1.67.0
underscore_BOOST_VERSION = boost_$(subst .,_,$(BOOST_VERSION))

PWD = $(shell pwd)
SRC_DIR = src
LIB_DIR = lib
TEST_DIR = test
INCLUDE_DIR = include
USR_LIB = /usr/local/lib/

MAIN_BINARIES = $(wildcard $(SRC_DIR)/*.cc)
SOURCE_BINARIES = $(filter-out $(SRC_DIR)/main.cc, $(MAIN_BINARIES))
TEST_BINARIES = $(wildcard $(TEST_DIR)/*.cc)

## 3rd party libraries
TINYXML2_BINARIES = $(wildcard $(LIB_DIR)/tinyxml2/*.cpp)
BOOST_DOWNLOAD_URL = https://dl.bintray.com/boostorg/release/$(BOOST_VERSION)/source/$(underscore_BOOST_VERSION).tar.gz
BOOST_DIR = $(USR_LIB)$(underscore_BOOST_VERSION)
# Includes
CATCH = -I ./$(LIB_DIR)/catch/
TINYXML2 = -I ./$(LIB_DIR)/tinyxml2/
BOOST = -I $(BOOST_DIR) -DBOOST_ERROR_CODE_HEADER_ONLY -lpthread
LIBS = $(CATCH) $(TINYXML2) $(BOOST)

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

server: build
	./main.out 4001

# Download and extract boost libraries
install:
	cd $(USR_LIB)
	wget $(BOOST_DOWNLOAD_URL)
	tar -xzf $(underscore_BOOST_VERSION).tar.gz
	cd $(PWD)

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
