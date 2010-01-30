#!/usr/bin/env python
# Some ideas:
# - use threading and server/client protocol
# - see if using a queue or a heap could be better for performances
# - write documentation in RST syntax for sphinx
# - use numpy for arrays
# - decouple RazzHand and Deck since they have different usage
# - see http://code.activestate.com/recipes/498229/ for the weighted choice

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
        self.initial_hands = {}
        self.initial_deck = Deck(DECK_CARDS)

        for p, h in init_cards.items():
            self.initial_hands[p] = RazzHand(h)
            # remove the hand given from the main deck
            self.initial_deck.remove(self.initial_hands[p])

        self.deck = deepcopy(self.initial_deck)
        self.hands = deepcopy(self.initial_hands)

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

    def reset(self):
        self.deck = deepcopy(self.initial_deck)
        self.hands = deepcopy(self.initial_hands)
        
    # Must remove also the initial cards from the working deck
    def loop(self, times, full = False):
        # at every loop it should restart from scratch
        ranks = {}
        for n in range(times):
            self.reset()
            got_rank = self.play()
            if ranks.has_key(got_rank):
                ranks[got_rank] += 1
            else:
                ranks[got_rank] = 1
        return ranks

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

# http://code.activestate.com/recipes/498233/ to fast lookup stuff
def toss(c):
    ###########################################################################
    # For example                                                             #
    # p=array((0.1, 0.2, 0.6, 0.1)) #vector of probabilities, normalized to 1 #
    # c=cumsum(p) #cumulative probability vector                              #
    ###########################################################################
    from numpy import cumsum, array, searchsorted, shape
    rd = numpy.random.random
    y = random(shape(c))
    x = searchsorted(c, y)
    

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

    s = """
    number of players: %d
    my initial cards: %s
    other players cards: %s""" % (nplayers, str(my_cards), str(other_cards))
    print s

    r = RazzGame(nplayers, init_cards)
    hist = r.loop(NROUNDS)
    makeHistogram(hist)
    
if __name__ == '__main__':
    main()
    
