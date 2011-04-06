#!/usr/bin/env python

"""
Get the theoretical probabilities only
given the initial cards dealt
"""

from razz import Deck, RazzHand, DECK_CARDS, Result, str_to_razz_card
from itertools import combinations
from sys import argv

def all_cards(init):
    """Generate ALL the possible combinations and
    find the REAL probabilities given"""
    d = Deck(DECK_CARDS)
    d.remove(init)
    ranks = {}

    count = 0
    for hand in combinations(d.cards, 4):
        count += 1
        h = RazzHand(list(hand) + init[:3])
        got_rank = h.rank()
        if ranks.has_key(got_rank):
            ranks[got_rank] += 1
        else:
            ranks[got_rank] = 1

    return ranks, count

if __name__ == '__main__':
    initial = map(str_to_razz_card, argv[1:])
    print Result(*all_cards(initial))
