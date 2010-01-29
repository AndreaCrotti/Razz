#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol
# - see if using a queue or a heap could be better for performances

import unittest
import random
from itertools import izip
from sys import argv

NROUNDS = 5
MAXPLAYER = 8
RAZZ_CARDS = dict(A = 1, J = 11, Q = 12, K = 13)

class RazzGame(object):
    """
    Main class representing the status of our game
    """
    DECK_CARDS = range(1, 14) * 4
    def __init__(self, nplayers):
        self.nplayers = nplayers

    def __str__(self):
        return "\n".join(map(str, self.hands.values()))

    def play(self):
        """ Run the game and return how many hands where below the rank given """
        for p, h in self.hands.items():
            while not(h.is_full()):
                # make sure to have all the cards
                self.addCardToPlayer(p, self.deck.getRandomCard())
            # check if the rank of my hand is equal to the given rank
        # at this point everybody should have 7 cards
        return self.hands[0].rank()

    def reset(self, init_cards):
        self.deck = Deck(self.DECK_CARDS)
        self.hands = {}
        for i in range(self.nplayers):
            self.hands[i] = RazzHand([])

        for k in init_cards.keys():
            for c in init_cards[k]:
                # in this way we also remove the initial cards from the deck
                self.addCardToPlayer(k, c)
        
    # Must remove also the initial cards from the working deck
    def loop(self, times, rank, init_cards):
        # at every loop it should restart from scratch
        counter = 0
        for n in range(times):
            self.reset(init_cards)
            if self.play() == rank:
                counter += 1
        print "got rank %d %d over %d times" % (rank, counter, times)

    def addCardToPlayer(self, player, card):
        self.hands[player].addCard(card)

class Deck(object):
    def __init__(self, card_list):
        self.cards = {}
        for c in card_list:
            self.addCard(c)

    def __str__(self):
        # I need to get back the card list to print correctly
        return str(self.cards)

    def __len__(self):
        return sum(self.cards.values())

    def getCard(self, card):
        "Returns one card or delete the entry when they're finished"
        if self.cards[card] == 1:
            self.cards.pop(card)
        else:
            self.cards[card] -= 1
        return card

    # FIXME: this random is not very fair, taking all the cards with different odds
    def getRandomCard(self):
        "Returns a card randomly from the deck"
        c = random.choice(self.cards.keys())
        return self.getCard(c)

    def addCard(self, card):
        if self.cards.has_key(card):
            self.cards[card] += 1
        else:
            self.cards[card] = 1

class RazzHand(Deck):
    """
    We don't take into account flush or straight
    and simply discard anything that is not a high card
    """
    TOT_CARDS = 7
    EVAL_CARDS = 5
    def __init__(self, card_list):
        super(RazzHand, self).__init__(card_list)
    
    # works only when everything has been normalized already
    def __cmp__(self, other):
        if self.has_duplicates():
            if other.has_duplicates():
                return 0
            else:
                return -1
        if other.has_duplicates():
            return 1

        cself, cother = sorted(self.cards.keys()), sorted(other.cards.keys())
        for s, o in izip(cself, cother):
            if s < o:
                return 1
            if s > o:
                return -1
        # only reaches here if they're perfectly equal
        return 0

    def rank(self):
        "Returns the rank of a high value hand"
        self.normalize()
        if self.has_duplicates():
            return -1
        else:
            return max(self.cards.keys())

    def is_full(self):
        return len(self) == self.TOT_CARDS

    # maybe better to not remove completely but just create another list
    def normalize(self):
        "Remove all the pairs, we are sure we're not removing too much thanks to has_duplicates"
        cards = self.cards.keys()
        done = lambda : len(self) == self.EVAL_CARDS

        for k in cards:
            while self.cards[k] > 1:
                if done():
                    return
                else:
                    self.getCard(k)

        cards.sort()
        while not(done()):
            # removing higher cards as much as possible
            c = cards.pop()
            self.getCard(c)

    def has_duplicates(self):
        "If I have 4 equal cards even removing 3 out of 7 I still get a pair"
        dups = sum(self.cards[k] - 1 for k in self.cards.keys())
        if dups > (self.TOT_CARDS - self.EVAL_CARDS + 1):
            return True
        else:
            return False

class TestRazzHand(unittest.TestCase):
    def test_highCardValues(self):
        h = [13, 10, 7, 6, 2]
        h1 = [13, 10, 7, 2, 1]
        self.assertTrue(RazzHand(h) < RazzHand(h1))

    def test_PairLosesAgainstHighCard(self):
        p1 = [13, 13, 7, 6, 2]
        p2 = [10, 7, 2, 1, 1]
        self.assertTrue(RazzHand(p1) < RazzHand(p2))

    def test_pokerLoseAgainstLowHand(self):
        pok = [6, 6, 6, 6, 3]
        low = [8, 4, 3, 2, 1]
        self.assertTrue(RazzHand(pok) < RazzHand(low))

    # adding some testing about normalization and more
    def test_NormalizeAlwaysGiveRightNumberOfCards(self):
        hand = RazzHand([1,2,2,3,4,5,5])
        hand2 = RazzHand([1,2,2,3,3])
        hand.normalize()
        hand2.normalize()
        self.assertTrue(len(hand) == hand.EVAL_CARDS)
        self.assertTrue(len(hand2) == hand2.EVAL_CARDS)
        

class TestDeck(unittest.TestCase):
    def test_RandomIsAlwaysGettingACard(self):
        r = Deck(range(1,14) * 4)
        i = 0
        # we get a card until the deck is empty
        while i < len(r):
            self.assertTrue(r.getRandomCard() > 0)
            i += 1
        
def str_to_card(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?

def main():
    nplayers = int(argv[1])
    my_cards = map(str_to_card, argv[2 : 5])
    other_cards = map(str_to_card, argv[5 : 5 + nplayers])
    init_cards = {}
    init_cards[0] = my_cards
    for p, o in enumerate(other_cards):
        init_cards[p] = [o]

    r = RazzGame(nplayers)
    r.loop(1000, 10, init_cards)
    
if __name__ == '__main__':
    # getting the arguments
    #unittest.main()
    main()
