#!/usr/bin/env python
# Some ideas:
# - use multiprocessing to split the work
#   from http://docs.python.org/dev/library/multiprocessing.html

from random import choice
from sys import argv
from collections import defaultdict

RAZZ_CARDS = dict(A=1, J=11, Q=12, K=13)
NON_HIGH_CARD = -1
DECK_CARDS = range(1, 14) * 4
RAZZ_HAND = 7


class RazzGame(object):
    """
    Main class representing the status of our game
    First give all the cards to each player,
    in the end we just care about the rank given by only one of
    the players
    """
    def __init__(self, nplayers, deck, init_cards):
        self.nplayers = nplayers
        self.hands = {}
        self.deck = deck
        for p, h in init_cards.items():
            self.hands[p] = RazzHand(h)
            self.deck.remove(self.hands[p].to_list())

    def give_cards(self):
        "Give cards to all player until they're hand is full"
        for n in range(self.nplayers):
            h = self.hands[n]
            while h.len < RAZZ_HAND:
                h.add_card(self.deck.get_random_card())

    def get_hand(self, player):
        return self.hands[player]


class Deck(object):
    def __init__(self, cards):
        # this copy is necessary otherwise we modify the DECK_CARDS given in input
        self.cards = cards[:]

    def remove(self, card_list):
        for c in card_list:
            assert(c in self.cards)
            self.cards.remove(c)

    # This is the function which must be maximally fast
    def get_random_card(self):
        "Returns a card randomly from the deck, assuming it's already in random order"
        c = choice(self.cards)
        self.cards.remove(c)
        return c


class RazzHand(object):
    """
    We don't take into account flush or straight
    and simply discard anything that is not a high card
    """
    TOT_CARDS = 7
    EVAL_CARDS = 5

    def __init__(self, card_list):
        self.cards = defaultdict(lambda: 1)
        self.len = 0
        for c in card_list:
            self.add_card(c)

    # ranking directly here turned out to be too complicated and also slower
    def add_card(self, card):
        self.cards[card] += 1
        self.len += 1

    # use sum or lambda instead
    def to_list(self):
        l = []
        for c, v in self.cards.items():
            l += [c] * v
        return l

    def rank(self):
        cards = self.cards.keys()
        if len(cards) < self.EVAL_CARDS:
            return NON_HIGH_CARD
        else:
            cards.sort()
            return cards[:self.EVAL_CARDS][-1]


def str_to_razz_card(s):
    try:
        return int(s)
    except ValueError:
        return RAZZ_CARDS[s]


class Result(object):
    CELL = 10

    def __init__(self, res, count, floating=False):
        self.result = res
        if not(floating):
            for k in self.result.keys():
                self.result[k] /= float(count)

    def __str__(self):
        res = []
        for k in sorted(self.result):
            s = str(k).ljust(self.CELL) + str(self.result[k]).ljust(self.CELL)
            res.append(s)
        return "\n".join(res)

    def to_arr(self):
        return [self.result[k] for k in sorted(self.result)]

# Must remove also the initial cards from the working deck
def loop(times, nplayers, init_cards, full=False):
    # at every loop it should restart from scratch
    ranks = defaultdict(lambda: 0)

    for n in range(times):
        # every time creating a new object
        d = Deck(DECK_CARDS)
        r = RazzGame(nplayers, d, init_cards)
        r.give_cards()
        got_rank = r.get_hand(0).rank()
        ranks[got_rank] += 1

    return ranks


# TODO: add better argument parsing 
def main():
    num_simulations = 10 ** int(argv[1])
    num_players = len(argv) - 4
    my_cards = map(str_to_razz_card, argv[2 : 5])
    other_cards = map(str_to_razz_card, argv[5:])
    # I can concatenate the initial cards in one hand only
    # for performance reasons
    init_cards = {}
    init_cards[0] = my_cards

    for i in range(1, num_players):
        init_cards[i] = [other_cards[i-1]]

    ranks = loop(num_simulations, num_players, init_cards)
    print Result(ranks, num_simulations)

if __name__ == '__main__':
    if len(argv) == 1:
        # nice trick to test automatically when not passing arguments
        import nose
        nose.run()
    else:
        main()
