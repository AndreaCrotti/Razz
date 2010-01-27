#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol

import unittest
import random
from itertools import product, izip
from sys import argv

# TODO: remove the ugly global stuff
# TODO: test everything better

NROUNDS = 5
MAXPLAYER = 8
RAZZ_CARDS = dict(A = 1, J = 11, Q = 12, K = 13)

class RazzGame(object):
    """
    Main class representing the status of our game
    """
    def __init__(self, ngame, players, deck):
        self.ngame = ngame
        self.players = players
        self.deck = deck

class Deck(object):
    def __init__(self, card_list):
        self.cards = {}
        self.card_list = card_list
        for c in card_list:
            # using setdefault?
            if self.cards.has_key(c):
                self.cards[c] += 1
            else:
                self.cards[c] = 1

    def getCard(self, card):
        "Returns one card or delete the entry when they're finished"
        if self.cards[card] == 1:
            self.cards.pop(card)
        else:
            self.cards[card] -= 1
        return card

    # in this way we choose only on the rank, not all the cards
    def getRandomCard(self):
        "Returns a card randomly from the deck"
        c = random.choice(self.cards.keys())
        return self.getCard(c)

    def addCard(self, card):
        if self.cards.has_key(card):
            self.cards[card] = 1
        else:
            self.cards[card] += 1
                

class RazzHand(Deck):
    def __init__(self, card_list):
        super(RazzHand, self).__init__(card_list)
    
    def __cmp__(self, other):
        # here is the smart part, must check for poker full or pair
        for s,o in izip(sorted(self.cards, reverse=True), sorted(other.cards, reverse=True)):
            if s < o:
                return 1
            if s > o:
                return -1
        return 0

    def bestHand(self):
        "Returns the best hand, which is the worst in razz games"
        pass
    
    def is_poker(self):
        return 4 in self.cards.values()

    def is_full(self):
        v = self.cards.values()
        return 3 in v and 2 in v

    def is_pair(self):
        return 2 in self.cards.values()

    def is_two_pair(self):
        "At least two pairs"
        return self.cards.values().count(2) >= 2

    
class Player(object):
    def __init__(self, name):
        self.name = name
        self.cards = []

    def addCard(self, card):
        self.cards.append(card)
    
class TestRazzTester(unittest.TestCase):
    def test_highCardValues(self):
        h = [13, 10, 7, 6, 2]
        h1 = [13, 10, 7, 2, 1]
        self.assertTrue(RazzHand(h) < RazzHand(h1))

    def test_Pair(self):
        p1 = [13, 13, 7, 6, 2]
        p2 = [10, 7, 2, 1, 1]
        self.assertTrue(RazzHand(p1) < RazzHand(p2))

    def test_pokerLoseAgainstLowHand(self):
        pok = [6, 6, 6, 6, 3]
        low = [8, 4, 3, 2, 1]
        self.assertTrue(RazzHand(pok) < RazzHand(low))

class TestDeck(unittest.TestCase):
    def test_deckNotEmpty(self):
        pass

def str_to_card(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?

if __name__ == '__main__':
    unittest.main()
    # getting the arguments
    idx = 1
    nplayers = int(argv[1])
    my_cards = map(str_to_card, argv[2 : 5])
    other_cards = map(str_to_card, argv[5 : 5 + nplayers])
    print nplayers, my_cards, other_cards
    
