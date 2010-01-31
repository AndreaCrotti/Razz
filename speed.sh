#!/bin/bash

# Timing the actual speed of our algorithm
# the importation time is not counted

python -m timeit -s "import razz" "razz.loop(1000, 4, { 0 : [1,1,2], 1:[2], 2:[3], 3:[4]})"