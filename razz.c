// -*- compile-command: "gcc -o  razz -Wall razz.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include "razz.h"

/*
  Problems:
  - see how to keep the structure of the deck
  - see how random could work on that structure in an efficient way

 */
#define N_SIM 1000 * 1000
#define RAZZ_HAND 7
#define RAZZ_EVAL 5

int *make_deck(const int, const int, const int);

// typedef must be written in the right order
typedef int card;

typedef struct deck {
  card *cards;
} deck; // using a pointer instead of the real struct?

typedef struct hand {
  // this is small can be allocated in stack memory
  card cards[RAZZ_HAND];
  int actual_len;
} hand;

// A card is just an integer

int main(int argc, char *argv[])
{
  hand h1 = {[1, 2, 3, 4], 4};
  
  return 0;
}

int hand_is_full(hand *hand) {
  if (hand -> actual_len == RAZZ_EVAL)
    return 0;
  return 1;
}

int *make_deck(const int start, const int end, const int rep) {
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
