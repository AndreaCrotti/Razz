#!/usr/bin/env python
# Some ideas:
# - see if using a queue or a heap could be better for performances
# - write documentation in RST syntax for sphinx
# - decouple RazzHand and Deck since they have different usage
# - use multiprocessing to split the work from http://docs.python.org/dev/library/multiprocessing.html
# - shuffling the deck every time is very costly but the only way to get a real randomization
# - try to prepare the deck already without the initial cards, deleting the ugly "remove" method

from random import choice
from sys import argv
from itertools import permutations

NROUNDS = 1000 * 100
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
    def __init__(self, nplayers, deck, init_cards):
        self.nplayers = nplayers
        self.hands = {}
        self.deck = deck

        for p, h in init_cards.items():
            self.hands[p] = RazzHand(h)
            self.deck.remove(self.hands[p].to_list())

    def __str__(self):
        return "\n".join(map(str, self.hands.values()))

    # TODO: insert 3 ways of playing, fair (with rounds), partial and fast
    def play(self, full = False):
        """ Run the game and return the rank of my local hand
        If full actually play the whole game not just my player.
        """
        if full:
            playing = self.nplayers
        else:
            playing = 1

        # this way is linear
        for n in xrange(playing):
            h = self.hands[n]
            while not(h.is_full()):
                h.addCard(self.deck.getRandomCard())

    def getHand(self, player):
        return self.hands[player]

class Deck(object):
    def __init__(self, cards):
        # this copy is necessary otherwise we modify the DECK_CARDS given in input
        self.cards = cards[:]
        
    def __str__(self):
        return str(self.cards)

    def __len__(self):
        return len(self.cards)

    def remove(self, card_list):
        for c in card_list:
            self.cards.remove(c)

    # This is the function which must be maximally fast
    def getRandomCard(self):
        "Returns a card randomly from the deck, assuming it's already in random order"
        c = choice(self.cards)
        # check that I'm actually removing the first one
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
        self.cards = {}
        for c in card_list:
            self.addCard(c)

    def __len__(self):
        return sum(self.cards.values())

    def __str__(self):
        return str(self.cards)

    def __cmp__(self, other):
        return cmp(self.rank(), other.rank())

    def addCard(self, card):
        if card in self.cards:
            self.cards[card] += 1
        else:
            self.cards[card] = 1

    # use sum or lambda instead
    def to_list(self):
        l = []
        for c, v in self.cards.items():
            l += [c] * v
        return l

    def rank(self):
        "Returns the rank of a high value hand"
        self.normalize()
        if self.has_duplicates():
            return NON_HIGH_CARD
        else:
            return max(self.cards.keys())

    def is_full(self):
        return len(self) == self.TOT_CARDS

    def getCard(self, card):
        if self.cards[card] > 1:
            self.cards[card] -= 1
        else:
            del(self.cards[card])
            return card

    def normalize(self):
        "Remove all the pairs, we are sure we're not removing too much thanks to has_duplicates"
        removed = 0
        to_remove = len(self) - self.EVAL_CARDS
        cards = self.cards.keys()

        for k in cards:
            while self.cards[k] > 1:
                if removed == to_remove:
                    return
                else:
                    # is this way the best one?
                    self.getCard(k)
                    removed += 1

        # duplicates are removed, now remove the highest cards
        cards.sort()
        # using cards.remove(r.max) looks a bit faster but if there are more
        # cards to remove the sorting must be done only once
        while removed != to_remove:
            # removing higher cards as much as possible
            c = cards.pop()
            self.getCard(c)
            removed += 1

    def has_duplicates(self):
        "Return if there is at least one duplicate card"
        #####################################################################
        # this form appear the fastest, faster then using                   #
        # any(map(lambda x: operator.gt(x, 1), d.values()))                 #
        # and also of a short circuiting function that quits at first entry #
        #####################################################################
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


# Must remove also the initial cards from the working deck
def loop(times, nplayers, init_cards, full = False):
    # at every loop it should restart from scratch
    ranks = {}

    for n in range(times):
        # every time creating a new object
        d = Deck(DECK_CARDS)
        r = RazzGame(nplayers, d, init_cards)
        r.play(full)
        got_rank = r.getHand(0).rank()

        if ranks.has_key(got_rank):
            ranks[got_rank] += 1
        else:
            ranks[got_rank] = 1
    return ranks

def get_rank5_tuples():
    rank5 = set()
    for n in range(100 * 1000):
        d = Deck(DECK_CARDS)
        r = RazzGame(1, d, {0 :[1, 2, 3]})
        r.play()
        h = tuple(r.getHand(0).to_list())
        got_rank = r.getHand(0).rank()
        if got_rank == 5:
            rank5.add(h)
    return rank5

def try_with_allcombinations():
    ranks = {}
    # we should in this way generate all the possible combinations
    # it is a way too slow even with 4 cards only taken 52! / (52-4)! or 49! / (49-4)!
    # if we remove first all the initial cards as it should be.
    for comb in permutations(range(5)*4, 4):
        d = Deck(list(comb))
        r = RazzGame(1, d, {0 :[1, 2, 3]})
        r.play()
        got_rank = r.getHand(0).rank()
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
    
