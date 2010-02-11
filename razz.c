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
  - computing the rank WHILE I'm adding cards to the deck
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - find where the 4 stupid bytes are  lost
  - use static variable instead of all those macros
*/

#define TO_EXP(x) powl(10, (x))

static Deck  global_deck;
static int   num_players;
static int   num_simulations;
static Card *to_remove;
static Hand  hand_init;
static Hand  hand_tmp;
static long  result[POSSIBLE_RANKS];


int main(int argc, char *argv[])
{
     init_hand(&hand_init);
     to_remove = malloc(sizeof(Card) * INITIAL_CARDS(num_players));
     get_args(argc, argv);

     qsort(to_remove, INITIAL_CARDS(num_players), sizeof(Card), intcmp);
     loop(&hand_init, result, to_remove);
     output_result(result);

     free(to_remove);
     return 0;
}

void get_args(int argc, char *argv[]) {
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
               add_card_to_hand(card, &hand_init);
          
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

void
loop(Hand *hand_init, long *result, Card to_remove[]) {
     int i, rank;
     
     // We use only ONE deck! Initialize it directly without the initial hand
     Deck *deck = &global_deck;
     init_deck(deck, 0, 13, 4, to_remove, INITIAL_CARDS(num_players));

     for (i = 0; i < num_simulations; i++) {
          deck->len = deck->orig_len;
          // restore the hand to the initial state at every loop
          hand_tmp = *hand_init;
          rank = play(deck, num_players, &hand_tmp);
          result[rank_to_result_idx(rank)]++;
     }
}

Card
play(Deck *deck, int nplayer, Hand *hand) {
     int card_idx;
  
     while (hand->len < RAZZ_HAND) {
          card_idx = get_random_card_from_deck(deck);
          add_card_to_hand(card_idx, hand);
     }
     return rank_hand(hand);
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

void
init_hand(Hand *hand) {
     hand->len = 0;
     hand->diffs = 0;
     memset(hand->cards, 0, sizeof(Card) * RAZZ_CARDS);
}

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
          
          if (rank_idx == RAZZ_EVAL) //? rewrite better this cycle
               break;
     }
     return IDX_TO_CARD(i);
}

// removing this malloc in favour of a static variable doensn't help at all
void
init_deck(Deck *deck, int start, int end, int rep, Card cards_to_remove[], int to_remove) {
     int i, j, idx, rem_idx;
     idx = rem_idx = 0;

     int len = (end - start) * rep - to_remove;
     deck->len = deck->orig_len = len;
  
     for (i = start; i < end; i++) {
          for (j = 0; j < rep; ) {
               while ((rem_idx < to_remove) && (i == cards_to_remove[rem_idx])) { // ? infinite loop with a printf... strange, is that really sorted??
                    assert(j < rep);
                    j++;
                    rem_idx++;
               }
               if (j++ < rep) {
                    deck->cards[idx++] = i;
               }
         }
     }
}

Card
get_random_card_from_deck(Deck *deck) {
     // using this instead of % is 40% faster, but results differ a bit!, using random even faster!
     int pos = (int) (deck->len * (random() / (RAND_MAX + 1.0)));
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

