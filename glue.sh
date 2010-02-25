#!/bin/bash
set -x
echo -e "theoretical result:"
./theory.py $@
echo -e "\nc program with 10^7 simulations:"
./razz 7 $@
echo -e "\npython program with 10^5 simulations:"
./razz.py 5 $@