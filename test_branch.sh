#!/bin/bash

echo "testing speed of current branch"
./speed.sh
git checkout exp

echo "testing sperimental"
./speed.sh

git checkout master
