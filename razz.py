#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol

##########################################################################################################################################################################################
# In Razz, there are 5 rounds.                                                                                                           #
#  Beside you as a player, the number of your opponents vary from 1 to 7.                                                                #
#  So, the maximum number of players sitting at a table is 8.                                                                            #
#  In round 0, each player is dealt 3 cards: 2 for each player's eyes only and 1 for everyone's eyes.                                    #
#  In round 0, your program should let the cards dealt for each player be determined by the user that runs the simulation.               #
#  IOW, the cards in round 0 are not drawn at random.                                                                                    #
#  In round 1, 2 and 3, each player is dealt one open card.                                                                              #
#  In the last round, each player is dealt 1 card for each player's eyes only.                                                           #
#  The user of the simulation should be able to specify a rank.                                                                          #
#  This rank is only a single rank: A, 2, 3, ..., K.                                                                                     #
#  There are no two pairs and the like.                                                                                                  #
#  Your simulation should simulate the game for n times (1 time == a complete round).                                                    #
#  Upon completing n times simulation, your program should report the number of hands that you have whose rank is <= the specified rank. #
#  That's all.                                                                                                                           #
#  HINT:                                                                                                                                 #
#  In round 0, all of the cards are preselected by the user.                                                                             #
#  Let's say that in round 0 there are X players.
#TIn the subsequent round, you only need to deal 1 card from (52 - X) cards (i.e., you don't deal your opponents any card at all because this won't change # the probability). #
#  For example, X is 5.                                                                                                                                                                                   
#  In round 0, 3 is for you and 4 is for your opponents (the hidden cards are not dealt).                                                                                                                 
#  So, in the deck, there is 52 - 3 - 4 cards left.                                                                                                                                                       
#  In round 1, you dealt 1 from 55.                                                                                                                                                                       
#  In round 2, you dealt 1 from 54.
#  And so on.
#  You deal no card for your opponent.
#  Why? Because your opponent cards won't change your odds, you just simply don't deal them for the sake of simplicity.#
#  That's all to it
# ##########################################################################################################################################################################################              

# http://www.pokereagles.com/poker-rules/razz-rules.php

import unittest
import random
import pprint
from itertools import product

# TODO: remove the ugly global stuff
# TODO: test everything better

NROUNDS = 5
MAXPLAYER = 8
PLAYERS_NAMES = [ "Aldo", "Bruno", "Carlo", "Dino", "Ezio", "Franco", "Gino" ]
RANKS = ["A"] + [ str(n) for n in range(2, 11) ] + ["J", "Q", "K"]
VALS = {}

i = 1
for c in RANKS:
    VALS[c] = i
    i += 1

SUITS = ["A", "B", "C", "D"]

def getNewCard(deck):
    while True:
        print "choose a card from the deck"
        pprint.pprint(deck)
        c = raw_input("value")
        s = raw_input("suit")
        card = deck.getCard(c, s)
        if card:
            return card
    

class RazzGame(object):
    """
    Main class representing the status of our game
    """
    def __init__(self, ngame, players, deck):
        self.ngame = ngame
        self.players = players
        self.deck = deck

    def round0(self):
        for p in self.players:
            print "selecting the hidden cards"
            for i in range(2):
                c = getNewCard(self.deck)
                p.addCard(c)

    def round123(self):
        pass

    def round5(self):
        pass


class RazzHand(object):

    def __init__(self, cards = []):
        self.cards = cards

    def __str__(self):
       return "; ".join(map(str, self.cards))

    def __cmp__(self, other):
        self.cards.sort(reverse=True)
        other.cards.sort(reverse=True)
        return cmp(self.cards, other.cards)

    def addCard(self, card):
        self.cards.append(card)

class Deck(object):
    """
    Class containig methods on the particular deck
    """
    def __init__(self, values, suits):
        self.cards = [ Card(val, suit) for val, suit in product(values, suits) ]

    def __str__(self):
        return "\n".join(map(str, self.cards))

    def getCard(self, value, suit):
        c = Card(value, suit)
        if c in self.cards:
            self.cards.remove(c)
            return c
        return None

    def getRandomCard(self):
        "Returns a card randomly from the deck"
        c = random.choice(self.cards)
        print "removing card %s" % str(c)
        self.cards.remove(c)
        return c

class Card(object):
    def __init__(self, value, suite):
        self.value = value
        self.suite = suite

    def __str__(self):
        return " of ".join([str(self.value), str(self.suite)])

    def __cmp__(self, other):
        return -cmp(VALS[self.value], VALS[other.value])

    def __eq__(self, other):
        return (self.value == other.value and self.suite == other.suite)
    
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

def gen_rand_hand():
    d = Deck(RANKS, SUITS)
    h = RazzHand()
    for _ in range(7):
        h.addCard(d.getRandomCard())
    return h

if __name__ == '__main__':
    pass

