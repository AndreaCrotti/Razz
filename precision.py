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
from theory import all_cards
from razz import DECK_CARDS, Result
from pylab import hist, clf, show
from itertools import combinations

to_c_args = {
    1 : 'A',
    11 : 'J',
    12 : 'Q',
    13 : 'K'
}

def init_to_c_args(cards):
    for i in range(len(cards)):
        if to_c_args.has_key(cards[i]):
            cards[i] = to_c_args[cards[i]]
        else:
            cards[i] = str(cards[i])
    return " ".join(cards)

def check_correctness(num_sim):
    for a,b,c in combinations(DECK_CARDS, 3):
        cprog = parse_c_result(str(num_sim) + " " + init_to_c_args([a,b,c]))
        dist = results_distance(Result(*all_cards([a,b,c])), Result(cprog, 10 ** num_sim, floating = True))
        print " ".join(map(str, [a,b,c,dist]))


def parse_c_result(args_list):
    result = {}
    s = "./razz_fast " + args_list
    out = popen(s)
    for l in out.readlines():
        rank, num = split(l, ":\t")
        result[int(rank)] = float(num)
    return result

def results_distance(res1, res2):
    "Printing the distance between the results given in input, using the norm"
    return np.linalg.norm(np.array(res1.to_arr()) - np.array(res2.to_arr()))

def hist_result(result):
    clf()
    hist(result.keys(), weights=result.values(), align='mid', bins=14, range=(-1,13))
    show() # never closing in this way

if __name__ == '__main__':
    # hist(times, weights=range(1,9))
    if len(argv) == 2:
        check_correctness(int(argv[1]))
    else:
        print "usage: ./precision.py <#simulations>"
        
