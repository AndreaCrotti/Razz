#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol

import unittest
import random
import pprint
from itertools import product

# TODO: remove the ugly global stuff
# TODO: test everything better

NROUNDS = 5
MAXPLAYER = 8

class RazzGame(object):
    """
    Main class representing the status of our game
    """
    def __init__(self, ngame, players, deck):
        self.ngame = ngame
        self.players = players
        self.deck = deck


class Deck(object):
    """
    Class containig methods on the particular deck
    """
    def __init__(self, values, repetitions):
        self.cards = dict(zip(values, [repetitions] * len(values)))

    def getCard(self, card):
        if self.cards.has_key(card) and self.cards[card] > 0:
            self.cards[card] -= 1
            return card
        return None

    ## When arriving to 0 card delete completely the entry
    def getRandomCard(self):
        "Returns a card randomly from the deck"
        c = random.choice(self.cards)
        print "removing card %s" % str(c)
        return c
    
class Player(object):
    def __init__(self, name):
        self.name = name
        self.cards = []

    def addCard(self, card):
        self.cards.append(card)

    
class TestRazzTester(unittest.TestCase):
    def test_AtLeastOnePlayer(self):
        pass

class TestDeck(unittest.TestCase):
    def test_deckNotEmpty(self):
        pass

if __name__ == '__main__':
    pass

