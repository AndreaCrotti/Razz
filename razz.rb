#!/usr/bin/env ruby -w
# -*- coding: undecided -*-

=begin rdoc
  simulation for Razz game
=end

=begin rdoc
  A deck is a colection of cards, and a hand is just of subset of it.
=end
class Deck
  def initialize(ranks, rep)
    @cards = []
    ranks.each { |x|
      1.upto(rep).each {
        @cards << x
      }
    }
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

require 'test/unit'

class TC_Razz < Test::Unit::TestCase
  attr_reader :r
  def setup
    @r = RazzGame.new %w[a b c]
  end

  def test_PlayersMoreThanZero
    assert(@r.players.length > 0)
  end
end


p Deck.new((%w[A J Q K].concat((2..10).to_a)), 4)
