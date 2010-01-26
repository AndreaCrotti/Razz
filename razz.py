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
    def __init__(self, values, repetitions):
        self.cards = dict(zip(values, [repetitions] * len(values)))

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

