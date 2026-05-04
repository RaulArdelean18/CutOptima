#!/usr/bin/env bash
set -e

g++ -std=c++17 -I. tests.cpp $(find Domain Repository Service -name '*.cpp' | sort) -o cutoptima_tests
./cutoptima_tests
