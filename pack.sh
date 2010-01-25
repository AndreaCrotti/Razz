#!/bin/bash

## documentation
rdoc
#ant doc
# Using sphinx with python

## Finally tarring up everything
FNAME=andrea_crotti_proj.tar.gz
FILES="*sh *rb *java *c *py *hs *erl doc"

tar -cvzf $FNAME $FILES