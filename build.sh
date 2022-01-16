#!/bin/bash

g++ main.cpp -o calc -O3 \
	-std=c++17 -Iinclude -fno-exceptions \
	-Wall -Wextra -Wno-attributes
