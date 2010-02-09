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
  - use static variable instead of all those macros
  - use static deck and hand, no dynamic allocation needed
*/

#define TO_EXP(x) powl(10, (x))

int getopt(int, char * const argv[], const char *);
extern char *optarg;

void parse_args(int, char * argv[]);

/* static Card global_hand[RAZZ_CARDS]; */

int main(int argc, char *argv[])
{
     if (argc < 6)
          usage();

     int nplayers, i, j, exp_args;
     Card card;
     // skip the program name

     long num_sims = TO_EXP(atol(argv[1]));

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
     
     Card *to_remove = malloc(sizeof(Card) * INITIAL_CARDS(nplayers));
     int rem_idx = 0;

     // divide issues 
     Hand **hands = malloc(sizeof(Hand *) * nplayers);
     hands[0] = make_hand();

     /// separating first player from the others
     for (i = 1; i < INITIAL_PLAYER+1; i++) {
          card = char_to_card_idx(argv[i+2][0]);
          add_card_to_hand(card, hands[0]);
          to_remove[rem_idx++] = card;
     }

     for (i = 1; i < nplayers; i++) {
          hands[i] = make_hand();
          for (j = 0; j < INITIAL_OTHER; j++) {
               card = char_to_card_idx(argv[i + INITIAL_PLAYER + 2][0]);
               add_card_to_hand(card, hands[i]);
               to_remove[rem_idx++] = card;
          }
     }

     qsort(to_remove, INITIAL_CARDS(nplayers), sizeof(Card), intcmp);
     long *result = malloc(sizeof(long) *POSSIBLE_RANKS);
     loop(num_sims, nplayers, hands, result, to_remove);
     output_result(result);
  
     free(result);

     for (i = 0; i < nplayers; i++)
          free_hand(hands[i]);

     free(to_remove);
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
     fprintf(stderr, "Usage: ./razz <k> <n> <c1_1> <c1_2> <c1_3> <c2_1> .. <cn_1>\n");
     fprintf(stderr, "\tk : exponent for number of simulations (10^k)\n");
     fprintf(stderr, "\tn : number of players\n");
     fprintf(stderr, "\tcx_y : yth card of xth player, player 1 wants 3 cards, other players only one.\n");
     fprintf(stderr, "\tfor example\n ./razz 6 2 A 2 3 4\nwill run 1 million simulations with 2 players\n");
     fprintf(stderr, "\twith Ace,2,3 for player 1 and 4 for player 2\n");
     fprintf(stderr, "\tthe returned statistics represent the number of times for each ranking obtained by player 1\n");
     exit(EX_USAGE);
}

void
loop(long simulations, int nplayers, Hand **init_hands, long *result, Card *to_remove) {
     int i, rank, idx;
     
     // now our wonderful deck already have deleted the unwanted cards
     Deck *d = make_deck(0, 13, 4, to_remove, INITIAL_CARDS(nplayers));

     Hand *h0 = make_hand();
     
     /// the deck I want to use is always without the initial hands, just do it
     for (i = 0; i < simulations; i++) {
          d->len = d->orig_len;

          copy_hand(init_hands[0], h0);
          rank = play(d, nplayers, h0);
          idx = rank_to_result_idx(rank);
          result[idx]++;
     }
     free_hand(h0);
     free_deck(d);
}

// write the content of h2 to h1, both must be already allocated
void
copy_hand(Hand *h1, Hand *h2) {
     memcpy(h2, h1, sizeof(Hand));
}

/// FIXME: now it's only taking h0, put also the various rounds
Card
play(Deck *d, int nplayer, Hand *h0) {
     int card_idx;
  
     while (h0->len < RAZZ_HAND) {
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
     return h;
}

// a possible nice solution to rank inside here directly would be to
// keep a stack of last maximums, when it's empty just keep the rank
// otherwise you can pop many times
void
add_card_to_hand(Card c, Hand *h) {
     if (!h->cards[c]) {
          h->diffs++;
     }
     h->cards[c]++;
     h->len++;
}

void
print_hand(Hand *h) {
     int i;
     for (i = 0; i < RAZZ_CARDS; i++)
          if (h->cards[i] > 0)
               printf("%d:\t%d\n", IDX_TO_CARD(i), h->cards[i]);
}


Card
rank_hand(Hand *h) {
     if (h->diffs < RAZZ_EVAL)
          return NON_HIGH_HAND;

     int rank_idx, i;
     rank_idx = 0;
     for (i = 0; i < RAZZ_CARDS; i++)  {
          if (h->cards[i])
               rank_idx++;
          
          if (rank_idx == RAZZ_EVAL)
               break;
     }
     return IDX_TO_CARD(i);
}

void free_hand(Hand *h) {
     free(h);
}

// removing this malloc in favour of a static variable doensn't help at all
Deck *
make_deck(int start, int end, int rep, Card init_cards[], int to_remove) {
     int i, j, idx, init_idx;
     idx = init_idx = 0;

     int len = (end - start) * rep - to_remove;

     Deck *deck = malloc(sizeof(Deck));

     deck->len = deck->orig_len = len;
  
     // FIXME: very convoluted triple cicle, try to make it nicer
     for (i = start; i < end; i++) {
          j = 0;
          for (;;) {
               while (i == init_cards[init_idx]) {
                    j++;
                    init_idx++;
               }

               assert(j <= rep); // kind of repetitive maybe?
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

void
print_deck(Deck *deck) {
     int i;
     for (i = 0; i < deck->len; i++) {
          printf("%d,",  deck->cards[i]);
     }
     printf("\n");
}

void
free_deck(Deck *deck) {
     /* free(deck->cards); */
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

int intcmp(const void *v1, const void *v2)
{
     return (*(int *)v1 - *(int *)v2);
}

