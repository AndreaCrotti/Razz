// -*- compile-command: "gcc -o  razz -Wall razz.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "razz.h"

/*
  Problems:
  - see how to keep the structure of the deck
  - see how random could work on that structure in an efficient way
  - use assertions

 */
#define N_SIM 1000 * 1000
#define RAZZ_HAND 7
#define RAZZ_EVAL 5

#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1

// A card is just an integer
typedef int card;

typedef struct deck {
  card *cards;
} deck; // using a pointer instead of the real struct?

typedef struct hand {
  // this is small can be allocated in stack memory
  card cards[RAZZ_HAND];
  int actual_len;
} hand;

card *make_deck(const int, const int, const int);
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
  exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (nplayers - 1)) + 2;

  // checking consistency between number of players and arguments
  if (exp_args != argc) {
    printf("wrong number of arguments, should be %d and its %d\n", exp_args, argc);
    exit(EXIT_FAILURE);
  }

  // creating a new deck here 
  for (i = 1; i < argc; i++) {
    if (strlen(argv[i]) > 1) {
      printf("use J, Q, K instead of 11, 12 or 13\n");
      exit(EXIT_FAILURE);
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
  
  return 0;
}

void start_game() {
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
  hand *h = (hand *) malloc(sizeof(hand));
  h -> actual_len = 0;
  return h;
}

void add_card_to_hand(card c, hand *h) {
  if (h->actual_len == RAZZ_HAND)
    printf("error");
  // FIXME: actual_len is not done correctly
  else {
    (h->cards)[h->actual_len] = c;
    h->actual_len++;
  }
}

void print_hand(hand *h) {
  int i;
  for (i = 0; i < h->actual_len; i++) 
    printf("%d ", h->cards[i]);
  printf("\n");
}

card *make_deck(const int start, const int end, const int rep) {
  int range_len = end - start;
  int i;
  int *deck = (int *) malloc(sizeof(int) * range_len);
  for (i = 0; i < range_len; i++) {
    deck[i] = rep;
  }

  return deck;
}

void remove_from_deck(int *deck, const int card) {
  if (deck[card - 1] > 0)
    deck[card - 1] -= 1;
  
  else
    printf("this should never happen");
}
