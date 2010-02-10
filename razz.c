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
*/

#define TO_EXP(x) powl(10, (x))

int getopt(int, char * const argv[], const char *);
void check_args(int, char **);

static Deck global_deck;
static int num_players;
static int num_simulations;
static Card *to_remove;
static Hand *hand_player0;
static long result[POSSIBLE_RANKS];


int main(int argc, char *argv[])
{
     hand_player0 = make_hand();
     to_remove = malloc(sizeof(Card) * INITIAL_CARDS(num_players));
     check_args(argc, argv);

     qsort(to_remove, INITIAL_CARDS(num_players), sizeof(Card), intcmp);
     loop(hand_player0, result, to_remove);
     output_result(result);

     free(to_remove);
     return 0;
}

void check_args(int argc, char *argv[]) {
     Card card;
     int i, j;

     if (argc < 6)
          usage();

     num_simulations = TO_EXP(atol(argv[1]));
     num_players = atoi(argv[2]);

     // max/min bound for players
     if (num_players > 8 || num_players < 1) {
          fprintf(stderr, "wrong number of players\n");
          usage();
     }
     int exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (num_players - 1)) + 3;
  
     // checking consistency between number of players and arguments
     if (exp_args != argc) {
          fprintf(stderr, "wrong number of cards given\n");
          usage();
     }

     for (i = 3, j = 0; i < argc; i++) {
          card = char_to_card_idx(argv[i][0]);
          if (j++ < INITIAL_PLAYER)
               add_card_to_hand(card, hand_player0);
          
          to_remove[i-3] = card;
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

// see if some global variables would be so bad
void
loop(Hand *init_h0, long *result, Card to_remove[]) {
     int i, rank;
     
     // We use only ONE deck!
     Deck *d = &global_deck;
     init_deck(d, 0, 13, 4, to_remove, INITIAL_CARDS(num_players));

     Hand *h0 = make_hand();
     
     /// the deck I want to use is always without the initial hands, just do it
     for (i = 0; i < num_simulations; i++) {
          d->len = d->orig_len;
          // restore the hand to the initial state at every loop
          *h0 = *init_h0;
          rank = play(d, num_players, h0);
          result[rank_to_result_idx(rank)]++;
     }
     free(h0);
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
     case '1' : return 9; // dirty trick, make sure we don't get 1 instead of A
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

// removing this malloc in favour of a static variable doensn't help at all
void
init_deck(Deck *deck, int start, int end, int rep, Card init_cards[], int to_remove) {
     int i, j, idx, init_idx;
     idx = init_idx = 0;

     int len = (end - start) * rep - to_remove;

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

