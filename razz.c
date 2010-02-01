// -*- compile-command: "gcc -o  razz -Wall razz.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "razz.h"

/*
  Problems:
  - see how to keep the structure of the deck
  - see how random could work on that structure in an efficient way

 */
#define N_SIM 1000 * 1000
#define RAZZ_HAND 7
#define RAZZ_EVAL 5

#define INITIAL_PLAYER 3
#define INITIAL_OTHERS 1

#define STR_EQ(x,y) (!strcmp(x, y))

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
card str_to_card(char *);
hand *make_hand();
void add_card_to_hand(card, hand *);
void print_hand(hand *);

int main(int argc, char *argv[])
{
  if (argc < 4) {
    exit(EXIT_FAILURE); // check the correct code
  }
  int nplayers, i;
  nplayers = atoi(argv[1]);

  // creating a new deck here 
  for (i = 1; i < argc; i++) {
    printf("got card from string %d\n", str_to_card(argv[i]));
  }
  hand *h0 = make_hand();
  add_card_to_hand(2, h0);
  print_hand(h0);
  

  return 0;
}

int str_to_card(char *card) {
  if (STR_EQ(card, "A")) {
    return 1;
  }
  if (STR_EQ(card, "J")) {
    return 11;
  }  
  if (STR_EQ(card, "Q")) {
    return 12;
  }
  if (STR_EQ(card, "K")) {
    return 13;
  }  
  // means that it's an integer
  return atoi(card);
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
    printf("now actual_len = %d", h->actual_len);
  }
}

void print_hand(hand *h) {
  int i;
  for (i = 0; i < h->actual_len; i++)
    printf("%d ", h->cards[i]);
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
