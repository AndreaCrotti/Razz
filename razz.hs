{-
  - Use a monad to encapsulate the changes.
-}

module Razz where

type Value = String
type Suit = String

data Card = Card Value
type Deck = [Card]
type Hand = [Card]

instance Show Card where
    show (Card v) = v
   
-- cartesian product of the input
mkDeck :: [Value] -> Int ->  Deck
mkDeck sx rep = concat $ replicate rep $ [ Card s | s <- sx]

razzCards = "A" : map show [2..10] ++ ["J", "Q", "K"]
razzDeck = mkDeck razzCards 4

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

