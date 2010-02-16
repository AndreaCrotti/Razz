#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysexits.h>
#include <math.h>
#include <pthread.h>
#include "razz.h"

/*
  Problems:
  - Using const whenever possible will increase the performances? No but it's more clean
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - use couples instead of diffs for a better usage
*/

#define TO_EXP(x) powl(10, (x))
/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)
#define INITIAL_CARDS(x) (INITIAL_PLAYER + (INITIAL_OTHER * (x - 1)))

#define NUM_THREADS 2

// instead of needing allocation
static Game game_conf;

int main(int argc, char *argv[])
{
     int i;
     /* srandom ((int)(time (NULL))); */
     init_hand(&game_conf.hand_init);
     prepare_game(argc, argv, &game_conf);

     Game game_threads[NUM_THREADS];
     pthread_t thread[NUM_THREADS];
     
     for (i = 0; i < NUM_THREADS; i++) {
          // simple approach, just copy the whole structure every time
          game_threads[i] = game_conf;
          printf("startin thread %d\n", i);
          pthread_create(&thread[i], NULL, loop, (void *)&game_threads[i]); 
     }

     for (i = 0; i < NUM_THREADS; i++) {
          pthread_join(thread[i], NULL);
          printf("ended thread %d\n", i);
          output_result(game_threads[i].result);
          /* free(&game_threads[i].to_remove); */
     }
     return 0;
}

// this should create the configuration for the game
// to pass around later also to the thread
void
prepare_game(int argc, char *argv[], Game *game) {
     Card card;
     int i, j;

     if (argc < 6)
          usage();

     game->num_simulations = (long) TO_EXP(atol(argv[1])) / NUM_THREADS;
     game->num_players = atoi(argv[2]);

     int rem_num = INITIAL_CARDS(game->num_players);
     game->to_remove = malloc(sizeof(Card) * rem_num);

     // max/min bound for players
     if (game->num_players > 8 || game->num_players < 1) {
          fprintf(stderr, "wrong number of players\n");
          usage();
     }
     int exp_args = INITIAL_PLAYER + (INITIAL_OTHER * (game->num_players - 1)) + 3;
  
     // checking consistency between number of players and arguments
     if (exp_args != argc) {
          fprintf(stderr, "wrong number of cards given\n");
          usage();
     }

     for (i = 3, j = 0; i < argc; i++) {
          card = char_to_card_idx(argv[i][0]);
          if (j++ < INITIAL_PLAYER)
               add_card_to_hand(card, &game->hand_init);
          
          game->to_remove[i-3] = card;
     }
     qsort(game->to_remove, rem_num, sizeof(Card), card_cmp);
     // should be done once for all
     init_deck(&game->deck, RAZZ_CARDS, RAZZ_REP, game->to_remove, INITIAL_CARDS(game->num_players));
}

void *
loop(void *game) {
     int i, rank;
     Game* pGame = (Game *)game;
     
     // We use only ONE deck! Initialize it directly without the initial hand
     Deck *deck = &pGame->deck;
     Hand hand_tmp; // should this maybe be part of a structure?

     for (i = 0; i < pGame->num_simulations; i++) {
          deck->len = deck->orig_len;
          // restore the hand to the initial state at every loop
          hand_tmp = pGame->hand_init;
          rank = give_and_rank(deck, &hand_tmp);
          assert(rank != 0); // different from 0
          pGame->result[rank_to_result_idx(rank)]++;
     }
     return NULL;
}

int
give_and_rank(Deck *deck, Hand *hand) {
     int card_idx;

     while (hand->len < RAZZ_HAND) {
          card_idx = get_random_card_from_deck(deck);
          add_card_to_hand(card_idx, hand);

          // detecting possible "couples" in an earlier stage, improvement is dependent
          // on MAX_COUPLES value
          if ((hand->len > (RAZZ_EVAL - MAX_COUPLES)) &&
              ((hand->len - hand->diffs) > MAX_COUPLES))
               return NON_HIGH_HAND;
     }
     return rank_hand(hand);
}
void
init_hand(Hand *hand) {
     hand->len = 0;
     hand->diffs = 0;
     memset(hand->cards, 0, sizeof(Card) * RAZZ_CARDS);
}

void
add_card_to_hand(Card c, Hand *hand) {
     if (!hand->cards[c])
          hand->diffs++;

     hand->cards[c]++;
     hand->len++;
}

Card
rank_hand(Hand *hand) {
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
// FIXME: line too long, give less arguments, mm no way if we also want to remove on the fly
void
init_deck(Deck *deck, int num_cards, int rep, Card cards_to_remove[], int to_remove) {
     Card i;
     int j, idx, rem_idx;
     idx = rem_idx = 0;

     int len = (num_cards * rep) - to_remove;
     deck->len = deck->orig_len = len;
  
     for (i = 0; i < num_cards; i++) {
          for (j = 0; j < rep; ) {
               while ((i == cards_to_remove[rem_idx]) && (rem_idx < to_remove)) {
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
     Card card = deck->cards[pos];
     // swap found element with the last one and then shrink the deck
     swap_cards(pos, deck->len-1, deck->cards);
     deck->len--;
     return card;
}

void
swap_cards(int c1_idx, int c2_idx, Card *cards) {
     Card tmp;
     tmp = cards[c1_idx];
     cards[c1_idx] = cards[c2_idx];
     cards[c2_idx] = tmp;
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
output_result(long *result) {
     int i;
     for (i = 0; i < POSSIBLE_RANKS; i++)
          printf("%d:\t%ld\n", idx_to_rank(i), result[i]);
}

int card_cmp(const void *v1, const void *v2)
{
     return (*(Card *)v1 - *(Card *)v2);
}

void
usage() {
     fprintf(stderr, "Usage: ./razz <k> <n> <c1_1> <c1_2> <c1_3> <c2_1> .. <cn_1>\n");
     fprintf(stderr, "\tk : exponent for number of simulations (10^k)\n");
     fprintf(stderr, "\tn : number of players\n");
     fprintf(stderr, "\tcx_y : yth card of xth player, player 1 wants 3 cards, other players only one.\n");
     fprintf(stderr, "\tfor example\n\t ./razz 6 2 A 2 3 4\n\twill run 1 million simulations with 2 players\n");
     fprintf(stderr, "\twith Ace,2,3 for player 1 and 4 for player 2\n");
     fprintf(stderr, "\tthe returned statistics represent the number of times for each ranking obtained by player 1\n");
     exit(EX_USAGE);
}
