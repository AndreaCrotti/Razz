#!/usr/bin/env python

# compare different results and graph the precision obtained
# from them
# there are various different configurations
# - python program with full game and no cheating
# - python program with partial game
# - c program with all the tricks on

# compare those results, compute the norm and so on

import numpy as np
from sys import argv
from os import popen
from string import split
from pylab import hist, clf, show
from razz import loop
LEN = 10000

def parse_c_result(args_list):
    result = {}
    s = "./razz_fast " + args_list
    print "executing", s
    out = popen(s)
    for l in out.readlines():
        print l
        rank, num = split(l, ":\t")
        result[int(rank)] = int(num)
    return result

def divide(res, num):
    for k in res:
        res[k] /= float(num)
    return res

r1 = divide(loop(LEN, 2, {0: [1,2,3], 1: [2]}), LEN)
r2 = divide(loop(LEN, 2, {0: [1,2,3], 1: [2]}), LEN)

def results_distance(res1, res2):
    dis = np.array([abs(res1[x] - res2[x]) for x in res1]) # automatically going through keys
    return np.linalg.norm(dis)

def hist_result(result):
    clf()
    hist(result.keys(), weights=result.values(), align='mid', bins=14, range=(-1,13))
    show() # never closing in this way

# run many times to construct a database of possible solutions and plot them on demand
        
if __name__ == '__main__':
    res = parse_c_result(" ".join(argv[1:]))
    hist_result(res)
    # print results_distance(r1, r2)
