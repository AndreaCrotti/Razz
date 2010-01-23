{-
  Create a sort of monad for playing somehow
-}

module Razz where

type Value = String
type Suit = String

data Card = Card Value Suit
type Deck = [Card]
type Hand = [Card]

instance Show Card where
    show (Card v s) = v ++ " of " ++ s
   
-- cartesian product of the input
mkDeck :: [Suit] -> [Value] -> Deck
mkDeck sx vx = [ Card s v | s <- sx, v <- vx ]

