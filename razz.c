// -*- compile-command: "make" -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysexits.h>
#include "razz.h"

/*
  Problems:
  - see how random could work on that structure in an efficient way
  - Using const whenever possible will increase the performances?
  - see if using static stuff could somehow help
  - use smaller types, like http://linux.die.net/man/3/uint8_t
  - fix problem with 10
  - decide which random generator to use (lrand, rand, random)
  - use j = 1 + (int) (10.0 * (rand() / (RAND_MAX + 1.0))) and not the modulo for the random
  - computing the cards WHILE I'm adding cards to the deck
  - things must be cleared after they've been used in the same place
 */


int main(int argc, char *argv[])
{
  if (argc < 4) {
    exit(EXIT_FAILURE); // check the correct code
  }

  int nplayers, i, j, exp_args;
  long nsims;
  char c;

  nsims = atol(argv[1]);
  nplayers = atoi(argv[2]);

  if (nplayers > 8 || nplayers < 1) {
    printf("wrong number of arguments\n");
    exit(EXIT_FAILURE);
  }
  exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (nplayers - 1)) + 3;
  
  // checking consistency between number of players and arguments
  if (exp_args != argc) {
    printf("wrong number of arguments, should be %d and its %d\n", exp_args, argc);
    exit(EX_USAGE);
  }

  // FIXME: not handling correctly the 10!!!
  // creating a new deck here 
  for (i = 2; i < argc; i++) {
    if (strlen(argv[i]) > 1) {
      printf("use J, Q, K instead of 11, 12 or 13\n");
      exit(EX_USAGE);
    }
  }

  // divide issues 
  hand **hands = malloc(sizeof(hand *) * nplayers);
  hands[0] = make_hand();

  /// separating first player from the others
  for (i = 1; i < INITIAL_PLAYER+1; i++) {
    c = char_to_card_idx(argv[i+2][0]);
    add_card_to_hand(c, hands[0]);
  }

  for (i = 1; i < nplayers; i++) {
    hands[i] = make_hand();
    for (j = 0; j < INITIAL_OTHER; j++) {
      c = char_to_card_idx(argv[i + INITIAL_PLAYER + 2][0]);
      add_card_to_hand(c, hands[i]);
    }
  }
  
  /* test_random_card(); */
  /* start_game(nplayers, hands); */
  add_card_to_hand(12, hands[0]);
  print_hand(hands[0]);
  /// freeing hands
  loop(nsims, nplayers, hands);
  for (i = 0; i < nplayers; i++)
    free_hand(hands[i]);

  return 0;
}

void loop(long simulations, int nplayers, hand **init_hands) {
  deck *d = make_deck(1, 14, 4);
  long counter = 0, i;
  
  /// the deck I want to use is always without the initial hands, just do it
  for (i = 0; i < simulations; i++) {
    d->len = d->orig_len;
    if ((play(d, nplayers, init_hands[0])) == 10)
      counter++;
  }

  printf("finally got %ld\n", counter);
  free(d);
}

card play(deck *d, int nplayer, hand *h0) {
  int card_idx;

  /* printf("len of deck = %d\n", d->len); */
  remove_hand_from_deck(h0, d);

  
  while (hand_is_full(h0)) {
    card_idx = get_random_card_from_deck(d);
    add_card_to_hand(card_idx, h0);
  }

  return rank_hand(h0);
}

void test_random_card() {
  int c, i;
  deck *d = make_deck(0, 4, 2);
  for (i = 0; i < 8; i++) {
    print_deck(d);
    c = get_random_card_from_deck(d);
    printf("at step %d got card %d\n", i, c);
  }
}

void test_hand_ranking() {
  int i;
  hand *h = make_hand();
  for (i = 0; i < 3; i++) {
    add_card_to_hand(i, h);
  }
  add_card_to_hand(8, h);
  add_card_to_hand(10, h);
  add_card_to_hand(10, h);
  add_card_to_hand(5, h);
  print_hand(h);
  printf("rank = %d\n", rank_hand(h));
}

int char_to_card_idx(char c) {
    switch ( c ) {
      case 'A': return 0;
      case 'J': return 10;
      case 'Q': return 11;
      case 'K': return 12;
      default : return CARD_TO_IDX(c - '0');
    }
}

hand *make_hand() {
  hand *h = malloc(sizeof(hand));
  h->len = 0;
  memset(h->cards, 0, sizeof(card) * RAZZ_CARDS);
  h->diffs = 0;
  h->rank = 0;
  return h;
}

/// modify the ranking inside here directly
void add_card_to_hand(card c, hand *h) {
  h->cards[c]++;
  h->len++;

  if (h->cards[c] > 0) {
    h->diffs++;
  }

  /// if we've never set the rank and we're full there are not enough different cards
  if (hand_is_full(h) && (!h->rank))
    h->rank = NON_HIGH_HAND;

  /// before we have enough different cards we take the max
  /// after we take the minimum (implicit discard of the higher cards)
  if (h->diffs < RAZZ_EVAL) {
    if (c > h->rank)
      h->rank = c;
  }
  else
    if (c < h->rank)
      h->rank = c;
}

int hand_is_full(hand *h) {
  if (h->len == RAZZ_HAND)
    return 0;
  else
    return 1;
}

void print_hand(hand *h) {
  int i;
  for (i = 0; i < RAZZ_CARDS; i++)
    if (h->cards[i] > 0)
      printf("%d:\t%d\n", IDX_TO_CARD(i), h->cards[i]);
}

/// even faster, goes backward in the array and grab the first one
card rank_hand(hand *h) {
  return h->rank;
  /* int i; */
  /* int to_remove = h->len - RAZZ_EVAL; */

  /* int higher=0; */
  /* for (i = 0; i < RAZZ_CARDS; i++) { */
  /*   if (h->cards[i] > 1) { */
  /*     if (h->cards[i] > to_remove) { */
  /*       h->cards[i] -= to_remove; */
  /*       return NONHIGH_HAND; */
  /*     } */
  /*     else { */
  /*       to_remove -= h->cards[i]-1; */
  /*       h->cards[i] = 1; */
  /*     } */
  /*     higher = i; */
  /*   } */
  /* } */
  /* return higher; */

}
void free_hand(hand *h) {
  // nothing else because the array of cards is an automatic variable
  free(h);
}

/// This function has to know both the details of hand and deck
/// trying to decouple it would be better 
void remove_hand_from_deck(hand *h, deck *d) {
  int i, j;
  /// Double for loop removing the cards,
  /// it could also keep a counter and only remove until LEN
  for (i = 0; i < RAZZ_CARDS; i++) {
    for (j = 0; j < h->cards[i]; j++) {
      remove_card_from_deck(i, d);
    }
  }
}

// we can avoid to call an external add_card_to_deck given that we
// only add card here, after we remove only
deck *make_deck(const int start, const int end, const int rep) {
  int i, j, idx;
  int range_len = end - start;
  int len = range_len * rep;

  deck *deck = malloc(sizeof(deck));
  deck->cards = malloc(sizeof(card) * len);
  deck->len = len;
  deck->orig_len = len;
  
  idx = 0;
  for (i = start; i < end; i++) {
    for (j = 0; j < rep; j++) {
      deck->cards[idx] = i;
      idx++;
    }
  }
  return deck;
}

// deck should be kept ordered in reverse order
// and putting a 0 to the card will make 

void print_deck(deck *deck) {
  int i;
  for (i = 0; i < deck->len; i++) {
    printf("%d,",  deck->cards[i]);
  }
  printf("\n");
}

// Given we only need to remove certain cards in the round 0 even better would be
// to generate directly the deck without them, keeping it sorted
void remove_card_from_deck(card c, deck *deck) {
  int i;
  // - swap the found card with the last card
  // - decrease the array by 1
  // not assuming any order and doing a brute force scan could also work
  // Is this a fair algorithm given the uniform distribution I should have in theory?

  for (i = 0; i < deck->len; i++) {
    if (deck->cards[i] == c) {
      swap_cards(i, deck->len-1, deck->cards);
      deck->len--;
      return;
    }
  }
}

card get_random_card_from_deck(deck *deck) {
  int pos = lrand48() % deck->len;
  card c = deck->cards[pos];
  swap_cards(pos, deck->len-1, deck->cards);
  deck->len--;
  return c;
}

void swap_cards(int c1_idx, int c2_idx, card *cards) {
  card tmp;
  tmp = cards[c1_idx];
  cards[c1_idx] = cards[c2_idx];
  cards[c2_idx] = tmp;
}

void free_deck(deck *deck) {
  free(deck->cards);
  free(deck);
}
