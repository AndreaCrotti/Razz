#!/bin/bash
# speed comparison on the same case

EUS=$HOME/uni/aachen/scientific/razz_simulation/
ANDREA=$HOME/uni/aachen/scientific/solutions/final/

ARG_E="1000000 1 SA S2 S3"
ARG_A="1 A 2 3"

cd $EUS
git pull
make
time ./razz $ARG_E

cd $ANDREA
make
time ./razz $ARG_A