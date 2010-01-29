#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol
# - see if using a queue or a heap could be better for performances
# - write documentation in RST syntax for sphinx

import unittest
from random import choice
from sys import argv
from copy import copy

NROUNDS = 1000
RAZZ_CARDS = dict(A = 1, J = 11, Q = 12, K = 13)

## FIXME: Getting too many times 5, there must be a bug
class RazzGame(object):
    """
    Main class representing the status of our game
    First give all the cards to each player,
    in the end we just care about the rank given by only one of
    the players
    """
    DECK_CARDS = range(1, 14) * 4
    def __init__(self, nplayers):
        self.nplayers = nplayers
        self.hands = {}

    def __str__(self):
        return "\n".join(map(str, self.hands.values()))

    def play(self):
        """ Run the game and return the rank of my local hand """
        for p, h in self.hands.items():
            while not(h.is_full()):
                # make sure to have all the cards
                self.addCardToPlayer(p, self.deck.getRandomCard())
                #print "now player %d has cards %s" % (p, str(self.hands[p]))
        # at this point everybody have 7 cards, I can rank my own cards
        print self.hands[0]
        return self.hands[0].rank()

    def reset(self, init_cards):
        self.deck = Deck(self.DECK_CARDS)

        for p, h in init_cards.items():
            # FIXME: the bug is here, I'm taking an modified razzhand
            self.hands[p] = RazzHand(h)
            # remove the hand given from the main deck
            self.deck.remove(self.hands[p])
        
    # Must remove also the initial cards from the working deck
    def loop(self, times, init_cards):
        # at every loop it should restart from scratch
        ranks = {}
        for n in range(times):
            self.reset(init_cards)
            got_rank = self.play()
            if ranks.has_key(got_rank):
                ranks[got_rank] += 1
            else:
                ranks[got_rank] = 1
        return ranks

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

    def __add__(self, other):
        res = copy(self)
        for c in other.cards.keys():
            if res.cards.has_key(c):
                res.cards[c] += other.cards[c]
            else:
                res.cards[c] = other.cards[c]
        return res
    
    def __sub__(self, other):
        res = copy(self)
        for c in other.cards.keys():
            res[c] = self.cards[c] - other.cards[c]
        return res

    def remove(self, other):
        for c in other.cards.keys():
            self.cards[c] -= other.cards[c]
            # this could actually only happen if you try to ask for
            # more than 4 cards of the same value in calling the program
            assert(self.cards[c] >= 0)

    def getCard(self, card):
        "Returns one card or delete the entry when they're finished"
        if self.cards[card] == 1:
            self.cards.pop(card)
        else:
            self.cards[card] -= 1
        return card

    def toList(self):
        l = []
        for k in self.cards.keys():
            l += [k] * self.cards[k]
        return l

    # apparently we're getting the same odds with the fair and non fair algorithm
    def getRandomCard(self):
        "Returns a card randomly from the deck"
        #c = choice(self.cards.keys())
        c = choice(self.toList())
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
    
    def __cmp__(self, other):
        return cmp(self.rank(),  other.rank())

    def rank(self):
        "Returns the rank of a high value hand"
        self.normalize()
        if self.has_duplicates():
            return -1
        else:
            return max(self.cards.keys())

    def is_full(self):
        return len(self) == self.TOT_CARDS

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
        "Return if there is at least one duplicate card"
        return any(map(lambda x: x > 1, self.cards.values()))

class TestRazzHand(unittest.TestCase):
    def test_highCardValues(self):
        h = [13, 10, 7, 6, 2]
        h1 = [13, 10, 7, 2, 1]
        self.assertTrue(RazzHand(h) == RazzHand(h1))

    def test_PairLosesAgainstHighCard(self):
        p1 = RazzHand([13, 13, 7, 6, 2])
        p2 = RazzHand([10, 7, 3, 2, 1])
        self.assertTrue(p1 < p2)

    def test_PairHasDuplicates(self):
        p1 = RazzHand([13, 13, 7, 6, 2])
        p1.normalize()
        self.assertTrue(p1.has_duplicates() == True)

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

    def test_RazzHandRanksCorrectly(self):
        hand = RazzHand([6,4,3,2,1])
        self.assertTrue(hand.rank() == 6)

    def test_RankPairIsCorrect(self):
        self.assertTrue(RazzHand([1,1,2,2,2,5,6]).rank() == -1)

class TestDeck(unittest.TestCase):
    def test_RandomIsAlwaysGettingACard(self):
        r = Deck(range(1,14) * 4)
        i = 0
        # we get a card until the deck is empty
        while i < len(r):
            self.assertTrue(r.getRandomCard() > 0)
            i += 1

class TestRazzGame(unittest.TestCase):
    def test_resetIsReallyResetting(self):
        pass
        
def str_to_RazzCard(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?


myGame = RazzGame(4)
myCards = {
    0 : [10, 10, 10],
    1 : [],
    2 : [],
    3 : []
}

myGame.reset(myCards)


def main():
    nplayers = int(argv[1])
    my_cards = map(str_to_RazzCard, argv[2 : 5])
    other_cards = map(str_to_RazzCard, argv[5 : 5 + nplayers])
    # I can concatenate the initial cards in one hand only
    # for performance reasons
    init_cards = {}
    init_cards[0] = my_cards
    
    for i in range(1, nplayers):
        init_cards[i] = [other_cards[i-1]]

    r = RazzGame(nplayers)
    r.loop(10000, 7, init_cards)
    
if __name__ == '__main__':
    # getting the arguments
    #unittest.main()
    main()
    
