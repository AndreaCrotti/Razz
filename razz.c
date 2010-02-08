// -*- compile-command: "make" -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sysexits.h>
#include <math.h>
#include "razz.h"

/*
  Problems:
  - Using const whenever possible will increase the performances?
  - see if using static stuff could somehow help
  - fix problem with 10
  - use j = 1 + (int) (10.0 * (rand() / (RAND_MAX + 1.0))) and not the modulo for the random
  - computing the rank WHILE I'm adding cards to the deck
  - things must be cleared after they've been used in the same place
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - who allocates the memory should be the same that deallocate it (or use make/free for that purpose)
  - check for correct freeing mallocs are needed?
  - find where the 4 stupid bytes are  lost 
*/

#define TO_EXP(x) powl(10, (x))

int getopt(int, char * const argv[], const char *);
extern char *optarg;

void parse_args(int, char * argv[]);

int main(int argc, char *argv[])
{
 
     /* if (argc < 4) { */
     /*      exit(EXIT_FAILURE); // check the correct code */
     /* } */
     /* parse_args(argc, argv); */
  
     int nplayers, i, j, exp_args;
     long nsims;
     char c;

     nsims = TO_EXP(atol(argv[1]));

     nplayers = atoi(argv[2]);

     if (nplayers > 8 || nplayers < 1) {
          usage();
     }
     exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (nplayers - 1)) + 3;
  
     // checking consistency between number of players and arguments
     if (exp_args != argc) {
          usage();
     }

     // FIXME: not handling correctly the 10!!!
     // creating a new deck here 
     for (i = 2; i < argc; i++) {
          if (strlen(argv[i]) > 1) {
               usage();
          }
     }

     // divide issues 
     Hand **hands = malloc(sizeof(Hand *) * nplayers);
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
  
     long *result = malloc(sizeof(long) *POSSIBLE_RANKS);
     loop(nsims, nplayers, hands, result);
     output_result(result);
  
     free(result);

     for (i = 0; i < nplayers; i++)
          free_hand(hands[i]);

     return 0;
}

void
parse_args(int argc, char *argv[]) {
  int ch;
  int verbose = 0;
  int n_threads = 1;

  const char optstring[] = "j:v";
  while ((ch = getopt(argc, argv, optstring))) {

    switch (ch) {
    case 'v':
      verbose = 1 ;
      break;
    case 'j':
      n_threads = atoi(optarg);
      break;
    default:
        usage();
    }
  }
}

void usage() {
     fprintf(stderr, "usage error\n");
     exit(EX_USAGE);
}

void
loop(long simulations, int nplayers, Hand **init_hands, long *result) {
     int i, rank, idx;
     
     Card *to_remove = hands_to_array(init_hands, nplayers);
     // now our wonderful deck already have deleted the unwanted cards
     Deck *d = make_deck(0, 13, 4, to_remove, INITIAL_PLAYER + (INITIAL_OTHER * (nplayers - 1)));

     /* Hand *h0 = make_hand(); */

     free(to_remove);
     /// the deck I want to use is always without the initial hands, just do it
     for (i = 0; i < simulations; i++) {
          /// trick, don't reallocate or reset anything, just move to original position the length
          /* srand48 (time (NULL)); /// every time reseeding or we always get the same game */
          d->len = d->orig_len;
          // copy the initial hands somewhere or we get always the same game
          // better is to be able to create new hands from initial values (like the python program)
          /* copy_hand(init_hands[0], h0); */
          Hand *h0 = copy_hand(init_hands[0]);
          rank = play(d, nplayers, h0);
          idx = rank_to_result_idx(rank);
          result[idx]++;
          free_hand(h0);
     }
     free_deck(d);
}

// TODO: make it void and write directly to the structure instead, much faster
Hand *
copy_hand(Hand *h1/* , Hand *h2 */) {
     int i;
     Hand *new = make_hand();
     for (i = 0; i < RAZZ_CARDS; i++) {
          new->cards[i] = h1->cards[i];
     }
     new->len = h1->len;
     new->diffs = h1->diffs;

     for (i = 0; i < h1->diffs; i++)
          new->card_list[i] = h1->card_list[i];
     /* new->card_list = h1->card_list; */
     return new;
}

/// FIXME: now it's only taking h0, put also the various rounds
Card
play(Deck *d, int nplayer, Hand *h0) {
     int card_idx;
  
     while (! hand_is_full(h0)) {
          card_idx = get_random_card_from_deck(d);
          add_card_to_hand(card_idx, h0);
     }

     return rank_hand(h0);
}

int
rank_to_result_idx(int rank) {
     switch (rank) {
     case -1: return 0;
     default : return (rank - MIN_RANK + 1);
     }
}

int
idx_to_rank(int idx) {
     switch (idx) {
     case 0: return -1;
     default : return (idx + MIN_RANK -1);
     }
}

int
char_to_card_idx(char c) {
     switch ( c ) {
     case 'A': return 0;
     case 'J': return 10;
     case 'Q': return 11;
     case 'K': return 12;
     default : return CARD_TO_IDX(c - '0');
     }
}

Hand *
make_hand() {
     Hand *h = malloc(sizeof(Hand));
     h->len = 0;
     h->diffs = 0;

     memset(h->cards, 0, sizeof(Card) * RAZZ_CARDS);
     memset(h->card_list, 0, sizeof(int) * RAZZ_HAND);
     return h;
}

/// modify the ranking inside here directly
void
add_card_to_hand(Card c, Hand *h) {
     if (!h->cards[c]) {
          h->card_list[h->diffs++] = c;
     }
     h->cards[c]++;
     h->len++;
}

int
hand_is_full(Hand *h) {
     if (h->len == RAZZ_HAND)
          return 1;
     else
          return 0;
}

void
print_hand(Hand *h) {
     int i;
     printf("--\ndiffs = %d\nArray=\n", h->diffs);
     for (i = 0; i < h->diffs; i++)
          printf("%d,", h->card_list[i]);
     printf("\n");
     for (i = 0; i < RAZZ_CARDS; i++)
          if (h->cards[i] > 0)
               printf("%d:\t%d\n", IDX_TO_CARD(i), h->cards[i]);
}

Card
rank_hand(Hand *h) {
     int rank_idx;
     if (h->diffs < RAZZ_EVAL)
          return NON_HIGH_HAND;
     
     qsort(h->card_list, h->diffs, sizeof(Card), intcmp);
     rank_idx = h->diffs - (h->diffs - RAZZ_EVAL + 1);
     return IDX_TO_CARD(h->card_list[rank_idx]);
}

void free_hand(Hand *h) {
     free(h);
}

Card *
hands_to_array(Hand **hands, int num_hands) {
     int i, k, counter;
     int j = 0;
     int len = 0;
     
     for (i = 0; i < num_hands; i++) {
          len += hands[i]->len;
     }
     
     Card *cards = malloc(sizeof(Card) * len);

     // maybe you could also try to insert smartly without sorting after
     for (k = 0; k < num_hands; k++) {
          for (i = 0; i < RAZZ_CARDS; i++) {
               for (counter = hands[k]->cards[i]; counter > 0; counter--) {
                    cards[j++] = i;
               }
          }
     }
     qsort(cards, len, sizeof(Card), intcmp);
     return cards;
}

int intcmp(const void *v1, const void *v2)
{
     return (*(int *)v1 - *(int *)v2);
}


// we can avoid to call an external add_card_to_deck given that we
// only add card here, after we remove only
Deck *
make_deck(int start, int end, int rep, Card init_cards[], int to_remove) {
     assert(to_remove < 5);

     int i, j, idx, init_idx;
     idx = init_idx = 0;

     int range_len = end - start;
     int len = range_len * rep - to_remove;

     Deck *deck = malloc(sizeof(Deck));
     deck->cards = malloc(sizeof(Card) * len); // not allocating useless space

     deck->len = len;
     deck->orig_len = len;
  
     // FIXME: if cards to remove more than 4 could have problems
     for (i = start; i < end; i++) {
          j = 0;
          for (;;) {
               while (i == init_cards[init_idx]) {
                    j++;
                    init_idx++;
               }

               assert(j <= rep);
               if (j < rep) {
                    deck->cards[idx] = i;
                    idx++;
                    j++;
               }
               else
                    break;
          }
     }
     return deck;
}

// deck should be kept ordered in reverse order
// and putting a 0 to the card will make 
void
print_deck(Deck *deck) {
     int i;
     for (i = 0; i < deck->len; i++) {
          printf("%d,",  deck->cards[i]);
     }
     printf("\n");
}

// Given we only need to remove certain cards in the round 0 even better would be
// to generate directly the deck without them, keeping it sorted
void
remove_card_from_deck(Card c, Deck *deck) {
     int i;
     // - swap the found card with the last card
     // - decrease the array by 1
     // not assuming any order and doing a brute force scan could also work
     // Is this a fair algorithm given the uniform distribution I should have in theory?

     // FIXME: better maybe to keep it sorted somehow and use a binary search
     for (i = 0; i < deck->len; i++) {
          if (deck->cards[i] == c) {
               swap_cards(i, deck->len-1, deck->cards);
               deck->len--;
               return;
          }
     }
}

void
free_deck(Deck *deck) {
     free(deck->cards);
     free(deck);
}

// FIXME: using lower order bits, check if still ok
Card
get_random_card_from_deck(Deck *deck) {
     int pos = lrand48() % deck->len;
     Card c = deck->cards[pos];
     swap_cards(pos, deck->len-1, deck->cards);
     deck->len--;
     return c;
}

void
swap_cards(int c1_idx, int c2_idx, Card *cards) {
     Card tmp;
     tmp = cards[c1_idx];
     cards[c1_idx] = cards[c2_idx];
     cards[c2_idx] = tmp;
}

void
output_result(long *result) {
     int i;
     for (i = 0; i < POSSIBLE_RANKS; i++)
          printf("%d:\t%ld\n", idx_to_rank(i), result[i]);
}

void
merge_results(long *res1, long *res2) {
     int i;
     for (i = 0; i < POSSIBLE_RANKS; i++)
          res1[i] += res2[i];
}
