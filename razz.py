#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol
# - see if using a queue or a heap could be better for performances
# - write documentation in RST syntax for sphinx
# - use numpy for arrays
# - decouple RazzHand and Deck since they have different usage
# - see http://code.activestate.com/recipes/498229/ for the weighted choice

# MOVE OUTSIDE THE LOOP!!

from random import choice
from sys import argv
from copy import deepcopy

NROUNDS = 10000
RAZZ_CARDS = dict(A = 1, J = 11, Q = 12, K = 13)
NON_HIGH_CARD = -1
DECK_CARDS = range(1, 14) * 4

class RazzGame(object):
    """
    Main class representing the status of our game
    First give all the cards to each player,
    in the end we just care about the rank given by only one of
    the players
    """
    def __init__(self, nplayers, init_cards):
        self.nplayers = nplayers
        self.hands = {}
        self.deck = Deck(DECK_CARDS)

        for p, h in init_cards.items():
            self.hands[p] = RazzHand(h)
            # remove the hand given from the main deck
            self.deck.remove(self.hands[p])

    def __str__(self):
        return "\n".join(map(str, self.hands.values()))

    def play(self, full = False):
        """ Run the game and return the rank of my local hand
        If full actually play the whole game not just my player.
        """
        if full:
            playing = self.nplayers
        else:
            playing = 1

        for n in xrange(playing):
            h = self.hands[n]
            while not(h.is_full()):
                h.addCard(self.deck.getRandomCard())

        return self.hands[0].rank()


class Deck(object):
    def __init__(self, card_list):
        self.cards = {}
        for c in card_list:
            self.addCard(c)

        self.weights = lambda : [(k, float(v) / len(self)) for k,v in self.cards.items() ]

    def __str__(self):
        # I need to get back the card list to print correctly
        return str(self.cards)

    def __len__(self):
        return sum(self.cards.values())

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
        c = choice(self.cards.keys())
        #c = choice(self.toList())
        #c = w_choice(self.weights())
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
            return NON_HIGH_CARD
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

        
def str_to_RazzCard(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?

def makeHistogram(values):
    cell = 10
    print "RANK".ljust(cell) + "TIMES".ljust(cell)
    for k, v in values.items():
        s = str(k).ljust(cell) + str(v).ljust(cell)
        print s
    print "TOT".ljust(cell) + str(sum(values.values())).ljust(cell)

def w_choice(lst):
    """weighted probability, for example
    x = w_choice( [('one',0.25), ('two',0.25), ('three',0.5)] )"""

    import random
    n = random.uniform(0, 1)
    for item, weight in lst:
        if n < weight:
            break
        n = n - weight
    return item

# Must remove also the initial cards from the working deck
def loop(times, nplayers, init_cards, full = False):
    # at every loop it should restart from scratch
    ranks = {}
    for n in range(times):
        # every time creating a new object
        r = RazzGame(nplayers, init_cards)
        got_rank = r.play()
        if ranks.has_key(got_rank):
            ranks[got_rank] += 1
        else:
            ranks[got_rank] = 1
    return ranks

def main():
    if len(argv) == 1:
        # nice trick to test automatically when not passing arguments
        import nose
        nose.run()
        return

    nplayers = int(argv[1])
    my_cards = map(str_to_RazzCard, argv[2 : 5])
    other_cards = map(str_to_RazzCard, argv[5 : 5 + nplayers])
    # I can concatenate the initial cards in one hand only
    # for performance reasons
    init_cards = {}
    init_cards[0] = my_cards
    
    for i in range(1, nplayers):
        init_cards[i] = [other_cards[i-1]]

    s = """
    number of players: %d
    my initial cards: %s
    other players cards: %s""" % (nplayers, str(my_cards), str(other_cards))
    print s

    hist = loop(NROUNDS, nplayers, init_cards)
    makeHistogram(hist)
    
if __name__ == '__main__':
    main()
    
