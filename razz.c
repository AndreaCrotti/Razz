#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysexits.h>
#include <math.h>
#include "razz.h"

/*
  Problems:
  - Using const whenever possible will increase the performances? No but it's more clean
  - computing the rank WHILE I'm adding cards to the deck
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - find where the 4 stupid bytes are  lost
  - use static variable instead of all those macros
  - check if forking would be faster http://www.csl.mtu.edu/cs4411/www/NOTES/process/fork/wait.html
*/

#define TO_EXP(x) powl(10, (x))
/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)
#define INITIAL_CARDS(x) (INITIAL_PLAYER + (INITIAL_OTHER * (x - 1)))

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
          play(deck, num_players, &hand_tmp);
          rank = rank_hand(&hand_tmp);
          assert(rank); // different from 0
          result[rank_to_result_idx(rank)]++;
     }
}

void
play(Deck *deck, int nplayer, Hand *hand) {
     int card_idx;
  
     while (hand->len < RAZZ_HAND) {
          card_idx = get_random_card_from_deck(deck);
          add_card_to_hand(card_idx, hand);
     }
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
     if (!h->cards[c])
          h->diffs++;

     h->cards[c]++;
     h->len++;
}

Card
rank_hand(Hand *hand) {
     if (hand->diffs < RAZZ_EVAL)
          return NON_HIGH_HAND;

     int rank_idx, i;
     rank_idx = 0;
     
     // scanning the array in reverse order
     for (i = RAZZ_CARDS-1; i > 0; i--)  {
          if (hand->cards[i])
               rank_idx++;
          
          if (rank_idx == (hand->diffs - RAZZ_EVAL + 1))
               return IDX_TO_CARD(i);
     }
     return 0;
}

// This works at condition that the cards_to_remove array is sorted in ascending order
void
init_deck(Deck *deck, int start, int end, int rep, Card cards_to_remove[], int to_remove) {
     int i, j, idx, rem_idx;
     idx = rem_idx = 0;

     int len = (end - start) * rep - to_remove;
     deck->len = deck->orig_len = len;
  
     for (i = start; i < end; i++) {
          for (j = 0; j < rep; ) {
               while (i == cards_to_remove[rem_idx]) {
                    assert(j < rep);
                    j++;
                    rem_idx++;
               }
               // if still we have other i cards to add we can add them to the deck
               if (j++ < rep)
                    deck->cards[idx++] = i;
         }
     }
}

Card
get_random_card_from_deck(Deck *deck) {
     // using random() and / instead of % and lrand48() is 40% faster
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

int intcmp(const void *v1, const void *v2)
{
     return (*(int *)v1 - *(int *)v2);
}

