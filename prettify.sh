#!/bin/sh

clang-format --verbose -i -style=llvm ./src/*.c ./src/*.h ./engine/*.c ./engine/*.h
