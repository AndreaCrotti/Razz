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
int *make_deck(const int, const int, const int);

// A card is just an integer

int main(int argc, char *argv[])
{
  
  return 0;
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
