#!/usr/bin/env python
# Some ideas:
# - see if using a queue or a heap could be better for performances
# - write documentation in RST syntax for sphinx
# - use multiprocessing to split the work
#   from http://docs.python.org/dev/library/multiprocessing.html
# - try to prepare the deck already without the initial cards,
#   deleting the ugly "remove" method

from random import choice
from sys import argv

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
    STAGES = range(3, 8)
    # FIXME: move out responsabilities
    def __init__(self, nplayers, deck, init_cards, staged = True, full = False):
        # FIXME: this init is doing a way too many things
        self.nplayers = nplayers
        self.hands = {}
        self.deck = deck
        if not(staged):
            self.stages = [7]
        else:
            self.stages = range(3, 8)

        if full:
            self.playing = self.nplayers
        else:
            self.playing = 1

        for p, h in init_cards.items():
            self.hands[p] = RazzHand(h)
            self.deck.remove(self.hands[p].to_list())

    def __str__(self):
        return "\n".join(map(str, self.hands.values()))

    # cards are given until the stage is completed
    def give_cards(self, staged = False):
        ## staged and full should be given here not in the stupid init method
        for stage_cards in self.stages:
            for n in xrange(self.playing):
                h = self.hands[n]
                while h.len < stage_cards: # a bit dirtier than len() but 10% faster
                    h.addCard(self.deck.getRandomCard())
                    
    # Stages:
    # 0 -> 3 cards each (give 2 to other players)
    # 1,2,3 -> 1 card each
    # 4 -> 1 final card
    # every time remove the cards from the deck
    # We must first remove the cards, for fun determine also whos the winner of the game

    def getHand(self, player):
        return self.hands[player]

class Deck(object):
    def __init__(self, cards):
        # this copy is necessary otherwise we modify the DECK_CARDS given in input
        self.cards = cards[:]
        
    def __str__(self):
        return str(self.cards)

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
        self.len = 0
        for c in card_list:
            self.addCard(c)

    def __str__(self):
        return str(self.cards)

    def __cmp__(self, other):
        return cmp(self.rank(), other.rank())

    def __len__(self):
        return self.len

    # ranking directly here turned out to be too complicated and also slower
    def addCard(self, card):
        if card in self.cards:
            self.cards[card] += 1
        else:
            self.cards[card] = 1
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
        
def str_to_RazzCard(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?

class Result(object):
    CELL = 10
    def __init__(self, res, count):
        self.result = res
        for k in self.result.keys():
            self.result[k] /= float(count)

    def __str__(self):
        res = []
        ks = self.result.keys()
        ks.sort()
        for k in ks:
            s = str(k).ljust(self.CELL) + str(self.result[k]).ljust(self.CELL)
            res.append(s)
        return "\n".join(res)

# Must remove also the initial cards from the working deck
def loop(times, nplayers, init_cards, full = False):
    # at every loop it should restart from scratch
    ranks = {}

    for n in range(times):
        # every time creating a new object
        d = Deck(DECK_CARDS)
        r = RazzGame(nplayers, d, init_cards)
        r.give_cards()
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

    num_simulations = 10 ** int(argv[1])
    nplayers = int(argv[2])
    my_cards = map(str_to_RazzCard, argv[3 : 6])
    other_cards = map(str_to_RazzCard, argv[6 : 6 + nplayers])
    # I can concatenate the initial cards in one hand only
    # for performance reasons
    init_cards = {}
    init_cards[0] = my_cards
    
    for i in range(1, nplayers):
        init_cards[i] = [other_cards[i-1]]

    ranks = loop(num_simulations, nplayers, init_cards)
    print Result(ranks, num_simulations)
    
if __name__ == '__main__':
    main()
    
