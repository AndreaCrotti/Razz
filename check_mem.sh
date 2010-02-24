#!/bin/bash
make
CMD="./razz_prof 6 A 2 3 4"
valgrind --tool=memcheck --leak-check=full --show-reachable=yes $CMD