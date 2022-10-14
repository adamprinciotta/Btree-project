# Makefile for CPS222 Project 4

# Google Test must be cloned and built.  If it's not in the same parent
# directory (..), change GTEST_ROOT to the actual location.
GTEST_ROOT = ../googletest/googletest
GTEST_MAIN = $(GTEST_ROOT)/make/libgtest_main.a

# Compiler flags: extra warnings (-Wall), debugging (-g), and Google Test
# Use C 2011 (required by Google Test, and a good idea anyway)
CXXFLAGS = -std=c++11

# -Wall and -Wextra requests more warnings -- fix them for clean code
# -g compiles for debugging
CPPFLAGS = -I $(GTEST_ROOT)/include -Wall -Wextra -g

all:	project4 test

BTree.o:	BTree.h BTreeFile.h BTreeBlock.h
BTree_test.o:	BTree.h BTreeFile.h BTreeBlock.h
BTreeBlock.o:	BTreeFile.h BTreeBlock.h
BTreeFile.o:	BTreeFile.h BTreeBlock.h
project4.o:	BTree.h

SUPPORT_OBJ = BTreeFile.o BTreeBlock.o

project4: 	BTree.o $(SUPPORT_OBJ) project4.o
	g++ -o $@ $^

test:		BTree_test
	./BTree_test

BTree_test:	BTree.o $(SUPPORT_OBJ) BTree_test.o $(GTEST_MAIN)
	g++ -o $@ $^ -pthread
