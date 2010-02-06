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

NROUNDS = 1000 * 1000
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
            self.addCard(c) # check if this would be possible self.cards.setdefault(c, 1).__add__(1)

    def __len__(self):
        return sum(self.cards.values())

    def __str__(self):
        return str(self.cards)

    def __cmp__(self, other):
        return cmp(self.rank(), other.rank())

    # ranking directly here turned out to be too complicated and also slower
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

    def is_full(self):
        return len(self) == self.TOT_CARDS

    def rank(self):
        removed = 0
        to_remove = len(self) - self.EVAL_CARDS
        cards = self.cards.keys()

        for k in cards:
            while self.cards[k] > 1:
                if removed == to_remove:
                    return NON_HIGH_CARD
                else:
                    self.cards[k] -= 1
                    removed += 1

        # sorting the card and returning the last after slicing only what we want
        cards.sort()
        return cards[:self.EVAL_CARDS][-1]
        
def str_to_RazzCard(s):
    if s.isdigit():
        return int(s)
    else:
        s = s.upper()
        return RAZZ_CARDS[s] # not handling exceptions here?

def output_ranks(values):
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

def get_rankn_tuples(rank):
    tuples = set()
    for n in range(100 * 1000):
        d = Deck(DECK_CARDS)
        r = RazzGame(1, d, {0 :[1, 2, 3]})
        r.play()
        h = tuple(r.getHand(0).to_list())
        got_rank = r.getHand(0).rank()
        if got_rank == rank:
            tuples.add(h)
    return tuples

def main():
    if len(argv) == 1:
        # nice trick to test automatically when not passing arguments
        import nose
        nose.run()
        return

    num_simulations = long(argv[1])
    nplayers = int(argv[2])
    my_cards = map(str_to_RazzCard, argv[3 : 6])
    other_cards = map(str_to_RazzCard, argv[6 : 6 + nplayers])
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

    ranks = loop(num_simulations, nplayers, init_cards)
    output_ranks(ranks)
    
if __name__ == '__main__':
    main()
    
