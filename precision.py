#!/usr/bin/env python
"""
Take the exponent of the number of simulations and the initial
cards and run 10 times a comparison of the C and theoretical output.
With optionally initial cards given only run once.

Distance is given by the norm of the differences * 100
"""

import numpy as np
from sys import argv
from os import popen
from string import split
from theory import all_cards
from razz import DECK_CARDS, Result, str_to_RazzCard

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

def check_correctness(num_sim, triple = None):
    "Print out some random triples and the precision of the running C program"
    from random import choice
    def cycle(triple):
        cprog = run_and_parse_c(str(num_sim) + " " + init_to_c_args(triple))
        py_triple = map(str_to_RazzCard, triple)
        dist = results_distance(Result(*all_cards(py_triple)), Result(cprog, 10 ** num_sim, floating = True))
        print " ".join(map(str, triple +[dist]))

    if triple:
        cycle(triple)
    else:
        for _ in range(10):
            cycle([choice(DECK_CARDS) for _ in range(3)])

def run_and_parse_c(args_list):
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
    if len(argv) == 2:
        check_correctness(int(argv[1]))
    else:
        check_correctness(int(argv[1]), argv[2:])
        
