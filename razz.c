// -*- compile-command: "gcc -o  razz -Wall razz.c" -*-

/**
 * @file   razz.c
 * @author Andrea Crotti <andrea.crotti@rwth-aache.de>
 * @date   Wed Feb  3 12:56:52 2010
 * 
 * @brief  
 * 
 * 
 */

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
 */

#define N_SIM 1000 * 100
#define RAZZ_HAND 7
#define RAZZ_EVAL 5

#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1

#define RAZZ_CARDS 13
#define RAZZ_REP 4

#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

// A card is just an integer
typedef int card;
typedef char rem;

typedef struct deck {
  card *cards;
  int len;
} deck;

typedef struct hand {
  card cards[RAZZ_CARDS]; /**< dictionary idx -> occurrences */
  int actual_len;
} hand;


deck *make_deck(const int, const int, const int);
void print_deck(deck *);
void free_deck(deck *);
hand *make_hand();
void add_card_to_hand(card, hand *);
void print_hand(hand *);
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
  for (i = 1; i < INITIAL_PLAYER+1; i++) {
    c = char_to_card(argv[i][0]);
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
    print_hand(hands[i]);
  }
  
  // free everything here
  for (i = 0; i < nplayers; i++) 
    free(hands[i]);

  free(hands);

  // starting deck code
  deck *d = make_deck(1, 13, RAZZ_REP);
  print_deck(d);
  free_deck(d);

  return 0;
}

void start_game(int nplayer, hand **init_hands) {

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

hand *make_hand() {
  int i;
  hand *h = malloc(sizeof(hand));
  h -> actual_len = 0;
  for (i = 0; i < RAZZ_CARDS; i++) {
    h->cards[i] = 0;
  }
  return h;
}

void add_card_to_hand(card c, hand *h) {
  h->cards[CARD_TO_IDX(c)]++;
  h->actual_len++;
}

void print_hand(hand *h) {
  int i;
  for (i = 0; i < RAZZ_CARDS; i++)
    printf("%d:\t%d\n", IDX_TO_CARD(i), h->cards[i]);
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
    printf("%d -> %d\n", i, deck->cards[i]);
  }
}

void remove_card_from_deck(card c, deck *deck) {
  // we can assume it's sorted in inverse order
  
}

// use the bsearch native implementation instead
card bin_search(card c, deck *deck) {
  int i = 0;
  int j = deck->len;
  int mid = (i + j) / 2;
}

void free_deck(deck *deck) {
  printf("freeing the deck\n");
  free(deck->cards);
  free(deck);
}
