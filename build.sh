#!/usr/bin/env bash
set -e

g++ -std=c++17 -I. main.cpp $(find Domain Repository Service -name '*.cpp' | sort) -o cutoptima
