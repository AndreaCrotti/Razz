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

to_c_args = {
    1 : 'A',
    11 : 'J',
    12 : 'Q',
    13 : 'K'
}

def init_to_c_args(cards):
    "translate card list to c program arguments"
    for i in range(len(cards)):
        if to_c_args.has_key(cards[i]):
            cards[i] = to_c_args[cards[i]]
        else:
            cards[i] = str(cards[i])
    return " ".join(cards)

def check_correctness(num_sim):
    "Print out some random triples and the precision of the running C program"
    from random import choice
    while True:
        triple = [choice(DECK_CARDS) for _ in range(3)]
        cprog = parse_c_result(str(num_sim) + " " + init_to_c_args(triple[:]))
        dist = results_distance(Result(*all_cards(triple)), Result(cprog, 10 ** num_sim, floating = True))
        print " ".join(map(str, triple +[dist]))


def parse_c_result(args_list):
    result = {}
    s = "./razz_fast " + args_list
    out = popen(s)
    for l in out.readlines():
        rank, num = split(l, ":\t")
        result[int(rank)] = float(num)
    return result

def results_distance(res1, res2):
    "Printing the distance between the results given in input, using the norm multiplied by 100"
    return np.linalg.norm(np.array(res1.to_arr()) - np.array(res2.to_arr())) * 100

if __name__ == '__main__':
    # hist(times, weights=range(1,9))
    if len(argv) == 2:
        check_correctness(int(argv[1]))
    else:
        print "usage: ./precision.py <#simulations>"
        
