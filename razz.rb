#!/usr/bin/env ruby -w
# -*- coding: undecided -*-

=begin rdoc
  simulation for Razz game
=end

=begin rdoc
  A deck is a colection of cards, and a hand is just of subset of it.
=end
class Deck
  def initialize(suits, ranks)
    @cards = []
    for s in suits
      for r in ranks
        @cards << (Card.new s, r)
      end
    end
  end
  
  def to_s
    @cards.inject("") { |res, x| res + x.to_s + "\n" }
  end

  def <<(card)
    @cards << card
  end

  def delete(card)
    @cards.delete card
  end

  def [](index)
    @cards[index]
  end

end

class Card
  def initialize(suit, rank)
    @suit = suit
    @rank = rank
  end

  def to_s
    "#{@rank} of #{@suit}"
  end

end


class Player
  attr_accessor :cards
  # a hand is just an empty deck at worst
  def initialize(name, hand = Deck.new([], []))
    @name = name
    @cards = cards
    @hand = hand
  end

  def to_s
    "#{@name}" + cards.to_s
  end
end


class RazzGame
  attr_reader :players, :deck

  def initialize(players)
    @suit = "a".."d"
    @ranks = %w[A J Q K].concat((2..10).to_a)

    @players = players
    @deck = Deck.new @suit, @ranks
  end

end


require 'test/unit'

class TC_Razz < Test::Unit::TestCase
  attr_reader :r
  def setup
    @r = RazzGame.new %w[a b c]
  end

  def test_PlayersMoreThanZero
    assert(@r.players.length > 0)
  end

  def test_DeckCreatesCards
    assert(@r.deck[0].is_a? Card)
  end
end


