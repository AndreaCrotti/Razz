/**
 * @file   razz.c
 * @author Andrea Crotti <andrea.crotti.0@gmail.com>
 * @date   Fri Feb  5 00:58:49 2010
 * 
 * @brief  This is a razz simulation
 * 
 * 
 */

// -*- compile-command: "gcc -o  razz -Wall razz.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysexits.h>
#include "razz.h"

/*
  Problems:
  - see how to keep the structure of the deck
  - see how random could work on that structure in an efficient way
  - use assertions
  - Using const whenever possible will increase the performances?
  - see if using static stuff could somehow help
  - use smaller types, like http://linux.die.net/man/3/uint8_t
  - fix problem with 10
  - see for example here http://stanford.edu/~blp/writings/clc/shuffle.html for array shuffling
  - decide which random generator to use (lrand, rand, random)
 */

#define N_SIM 1000 * 10000
#define RAZZ_HAND 7
#define RAZZ_EVAL 5

#define NON_HIGH_HAND (-1)

#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1

#define RAZZ_CARDS 13
#define RAZZ_REP 4

/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

int i;

// A card is just an integer
typedef int card;
typedef char rem;

typedef struct deck {
  card *cards;
  int len;
} deck;

typedef struct hand {
  card cards[RAZZ_CARDS]; /**< dictionary idx -> occurrences */
  int len;
} hand;

void swap_cards(int, int, card *);

deck *make_deck(const int, const int, const int);
void print_deck(deck *);
void free_deck(deck *);
card get_random_card_from_deck(deck *);

void test_random_card();
void test_hand_ranking();
void play();
void loop();

hand *make_hand();
void add_card_to_hand(card, hand *);
void print_hand(hand *);
int hand_is_full(hand *);
void free_hand(hand *);
card rank_hand(hand *);

card char_to_card(char);


int main(int argc, char *argv[])
{
  if (argc < 4) {
    exit(EXIT_FAILURE); // check the correct code
  }

  int nplayers, i, j, exp_args;
  char c;

  nplayers = atoi(argv[1]);
  if (nplayers > 8 || nplayers < 1) {
    printf("wrong number of arguments\n");
    exit(EXIT_FAILURE);
  }
  exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (nplayers - 1)) + 2;
  
  // checking consistency between number of players and arguments
  if (exp_args != argc) {
    printf("wrong number of arguments, should be %d and its %d\n", exp_args, argc);
    exit(EX_USAGE);
  }

  // FIXME: not handling correctly the 10!!!
  // creating a new deck here 
  for (i = 1; i < argc; i++) {
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
    c = char_to_card(argv[i+1][0]);
    add_card_to_hand(c, hands[0]);
  }

  for (i = 1; i < nplayers; i++) {
    hands[i] = make_hand();
    for (j = 0; j < INITIAL_OTHER; j++) {
      c = char_to_card(argv[i + INITIAL_PLAYER + 1][0]);
      add_card_to_hand(c, hands[i]);
    }
  }
  for (i = 0; i < nplayers; i++) {
    printf("hand %d:\n", i);
  }
  
  /// freeing hands
  for (i = 0; i < nplayers; i++) 
    free_hand(hands[i]);

  free(hands);

  /* test_random_card(); */
  /* test_hand_ranking(); */
  loop();

  return 0;
}

void start_game(int nplayer, hand **init_hands) {

}

void test_random_card() {
  int c;
  deck *d = make_deck(1, 5, 2);
  for (i = 0; i < 8; i++) {
    print_deck(d);
    c = get_random_card_from_deck(d);
    printf("at step %d got card %d\n", i, c);
  }
}

void test_hand_ranking() {
  hand *h = make_hand();
  for (i = 1; i < 4; i++) {
    add_card_to_hand(i, h);
  }
  add_card_to_hand(8, h);
  add_card_to_hand(10, h);
  add_card_to_hand(10, h);
  add_card_to_hand(5, h);
  print_hand(h);
  //  printf("rank = %d\n", rank_hand(h));
}

card char_to_card(char c) {
    switch ( c ) {
      case 'A': return 1;
      case 'J': return 11;
      case 'Q': return 12;
      case 'K': return 13;
      default : return (c - '0');
    }
}

void play() {
  deck *d = make_deck(1, 13, RAZZ_REP);
  hand *h = make_hand();
  card c;

  while (hand_is_full(h)) {
    c = get_random_card_from_deck(d);
    add_card_to_hand(c, h);
  }
  //printf("rank obtained = %d\n", rank_hand(h));
  
  free_deck(d);
  free_hand(h);
}

void loop() {
  int i;
  for (i = 0; i < N_SIM; i++){
    //    printf("%d\n", i);
    play();
  }
}

hand *make_hand() {
  hand *h = malloc(sizeof(hand));
  h -> len = 0;
  for (i = 0; i < RAZZ_CARDS; i++) {
    h->cards[i] = 0;
  }
  return h;
}

void add_card_to_hand(card c, hand *h) {
  h->cards[CARD_TO_IDX(c)]++;
  h->len++;
}

int hand_is_full(hand *h) {
  if (h->len == RAZZ_HAND)
    return 0;
  else
    return 1;
}

void print_hand(hand *h) {
  for (i = 0; i < RAZZ_CARDS; i++)
    if (h->cards[i] > 0)
      printf("%d:\t%d\n", IDX_TO_CARD(i), h->cards[i]);
}

card rank_hand(hand *h) {
  int to_remove = h->len - RAZZ_EVAL;

  /// here we don't need the CARD_TO_IDX macro since we only check for duplicates
  for (i = 0; i < RAZZ_CARDS; i++) {
    while (h->cards[i] > 1) {
      if (to_remove == 0)
        return NON_HIGH_HAND;
      
      h->cards[i]--;
      h->len--;
      to_remove--;
    }
  }

  /// scan the whole array from the end and grab the to_remove_th element
  for (i = RAZZ_CARDS-1; i > 0; i--) {
    assert(h->cards[i] < 2); /**< if failing means that the previous part didn't work correctly */
    if (h->cards[i] > 0) {

      if (to_remove == 0)
        return IDX_TO_CARD(i);
      else
        to_remove--;
    }
    /// when equal to 0 implicitly go to previous card
  }

  return 0; /// never getting here
}

void free_hand(hand *h) {
  // nothing else because the array of cards is an automatic variable
  free(h);
}

// we can avoid to call an external add_card_to_deck given that we
// only add card here, after we remove only
deck *make_deck(const int start, const int end, const int rep) {
  int j, idx;
  int range_len = end - start;
  int len = range_len * rep;

  deck *deck = malloc(sizeof(deck));
  deck->cards = malloc(sizeof(card) * len);
  deck->len = len;
  
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
  for (i = 0; i < deck->len; i++) {
    printf("%d,",  deck->cards[i]);
  }
  printf("\n");
}

// Given we only need to remove certain cards in the round 0 even better would be
// to generate directly the deck without them, keeping it sorted
void remove_card_from_deck(card c, deck *deck) {
  // A nice way to do this could be
  // - swap the found card with the last card
  // - decrease the array by 1
  // not assuming any order and doing a brute force scan could also work
  // Is this a fair algorithm given the uniform distribution I should have?
  for (i = 0; i < deck->len; i++) {
    if (deck->cards[i] == c) {
      swap_cards(i, deck->len-1, deck->cards);
      deck->len--;
      return;
    }
  }
  printf("we should never reach this point, card %d not found\n", c);
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
