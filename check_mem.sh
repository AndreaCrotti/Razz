#!/bin/bash
make
CMD="./razz_prof 5 A 2 3"
valgrind -v --tool=memcheck --leak-check=full --show-reachable=yes $CMD
