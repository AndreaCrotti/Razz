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

razzCards = "A" : map show [2..10] ++ ["J", "Q", "K"]
razzSuits = ["X", "Y", "Z", "W"]
razzDeck = mkDeck razzCards razzSuits

getValue :: Value -> Int
getValue v = case v of
               "A" -> 1
               "J" -> 11
               "Q" -> 12
               "K" -> 13
               x -> (read x)::Int
{-
  Using immutable structures is not very good for perfomances here
-}

