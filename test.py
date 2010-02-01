import unittest
from razz import *

class TestRazzHand(unittest.TestCase):
    def test_highCardValues(self):
        h = [13, 10, 7, 6, 2]
        h1 = [13, 10, 7, 2, 1]
        self.assertEqual(RazzHand(h), RazzHand(h1))

    def test_PairLosesAgainstHighCard(self):
        p1 = RazzHand([13, 13, 7, 6, 2])
        p2 = RazzHand([10, 7, 3, 2, 1])
        self.assertTrue(p1 < p2)

    def test_PairHasDuplicates(self):
        p1 = RazzHand([13, 13, 7, 6, 2])
        p1.normalize()
        self.assertEqual(p1.has_duplicates(), True)

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
        self.assertEqual(hand.rank(), 6)

    def test_RankPairIsCorrect(self):
        self.assertEqual(RazzHand([1,1,2,2,2,5,6]).rank(), NON_HIGH_CARD)

class TestDeck(unittest.TestCase):
    def test_RandomIsAlwaysGettingACard(self):
        r = Deck(DECK_CARDS)
        i = 0
        # we get a card until the deck is empty
        while i < len(r):
            self.assertTrue(r.getRandomCard() > 0)
            i += 1

def test_wChoiceGivingRightValues():
    d = [(0, 0.3), (1, 0.4), (2, 0.299999999)]
    for _ in range(100):
        assert(w_choice(d) in range(3))

def test_wChoiceWorkingForDeck():
    d = Deck(DECK_CARDS)
    weights = [(k, float(v) / len(d)) for k,v in d.cards.items() ]
    for _ in range(100):
        assert(w_choice(weights) in d.cards)

def test_randChoiceWorkingForDeck():
    d = Deck(DECK_CARDS)
    for _ in range(100):
        assert(choice(d.toList()) in d.cards)
    
if __name__ == '__main__':
    unittest.main()
