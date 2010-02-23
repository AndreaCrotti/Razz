#!/bin/bash
echo "theoretical result:"
./theory.py $@
echo "c program with 10^7 simulations:"
./razz 7 $@
echo "python program with 10^5 simulations:"
./razz.py 5 $@