import unittest
from random import choice
from razz import RazzHand, Deck, DECK_CARDS, NON_HIGH_CARD, loop

class TestRazzHand(unittest.TestCase):
    def test_PairLosesAgainstHighCard(self):
        p1 = RazzHand([13, 13, 7, 6, 2])
        p2 = RazzHand([10, 7, 3, 2, 1])
        self.assertTrue(p1.rank() < p2.rank())

    def test_pokerLoseAgainstLowHand(self):
        pok = [6, 6, 6, 6, 3]
        low = [8, 4, 3, 2, 1]
        self.assertTrue(RazzHand(pok).rank() < RazzHand(low).rank())

    def test_RazzHandRanksCorrectly(self):
        hand = RazzHand([6,4,3,2,1])
        self.assertEqual(hand.rank(), 6)

    def test_RankPairIsCorrect(self):
        self.assertEqual(RazzHand([1,1,2,2,2,5,6]).rank(), NON_HIGH_CARD)

    def test_lenAttributeWorkingCorrectly(self):
        self.assertEqual(RazzHand([1,1,3,3,4,5,5]).len, 7)

class TestDeck(unittest.TestCase):
    def test_RandomIsAlwaysGettingACard(self):
        r = Deck(DECK_CARDS)
        i = 0
        # we get a card until the deck is empty
        while i < len(r.cards):
            self.assertTrue(r.get_random_card() > 0)
            i += 1

# some longer tests should not be started automatically with the rests
def test_ProbAlwaysSimilar():
    nsims = 10000
    threshold = nsims * 2 /100 # 2 % of difference is allowed
    ranks = loop(10000, 1, {0:[1,2,3]})
    right = {-1: 292,
             5: 692,
             6: 1170,
             7: 1473,
             8: 1563,
             9: 1414,
             10: 1252,
             11: 932,
             12: 743,
             13: 469}

    for r in ranks.keys():
        assert(abs(ranks[r] - right[r]) < threshold)
            # print ranks[r], right[r]

def test_randChoiceWorkingForDeck():
    d = Deck(DECK_CARDS)
    for _ in range(100):
        assert(choice(d.cards) in d.cards)

def test_Ranking5WorksCorrectly():
    "Checking some possible where I should get a rank5"
    l = [
     [1, 1, 2, 3, 4, 5, 9],
     [1, 1, 2, 3, 4, 5, 10],
     [1, 1, 2, 3, 4, 5, 11],
     [1, 1, 2, 3, 4, 5, 12],
     [1, 1, 2, 3, 4, 5, 13],
     [1, 2, 2, 2, 3, 4, 5],
     [1, 2, 2, 3, 3, 4, 5],
     [1, 2, 2, 3, 4, 4, 5],
     [1, 2, 2, 3, 4, 5, 6],
     [1, 2, 2, 3, 4, 5, 7],
     [1, 2, 2, 3, 4, 5, 8],
     [1, 2, 2, 3, 4, 5, 9],
    ]
    for tup in l:
        assert(RazzHand(tup).rank() == 5)



def test_RankNegativeWorks():
    l = [
        [1, 1, 2, 2, 2, 3, 4],
        [10, 10, 10, 1, 1, 4, 5],
    ]
    for tup in l:
        assert(RazzHand(tup).rank() == -1)

    
if __name__ == '__main__':
    unittest.main()
