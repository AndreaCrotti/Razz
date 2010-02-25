#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <math.h>
#include "razz.h"

/*
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - use couples instead of diffs for a better usage
  - adding another structure Simulation would be better?
  - check if possible to use a different random simulator, for example
    http://en.wikipedia.org/wiki/Multiply-with-carry
  - check if using vfork instead of fork would be better (does not copy the stack)
*/

#define TO_EXP(x) powl(10, (x))
#define INITIAL_CARDS(x) (INITIAL_PLAYER + (INITIAL_OTHER * (x - 1)))

// instead of needing allocation
static Game game_conf;
void free_conf(Game *);
unsigned int time(int);

int main(int argc, char *argv[])
{
     srandom(time (0));
     init_hand(&game_conf.hand_init);
     get_args(argc, argv, &game_conf);
     
     loop(&game_conf);
     output_result(game_conf.result, game_conf.num_simulations);
     return 0;
}

// this should create the configuration for the game
// to pass around later also to the thread
void
get_args(int argc, char *argv[], Game *game) {
     Card card;
     int i, j;

     if (argc < 5)
          usage();

     game->num_simulations = TO_EXP(atol(argv[1]));
     int num_players = argc - 4;

     int rem_num = INITIAL_CARDS(num_players);
     Card *to_remove = malloc(sizeof(Card) * rem_num);

     // max/min bound for players
     if (num_players > 8 || num_players < 1) {
          fprintf(stderr, "wrong number of players\n");
          usage();
     }

     for (i = 2, j = 0; i < argc; i++) {
          card = char_to_card_idx(argv[i][0]);
          if (j++ < INITIAL_PLAYER)
               add_card_to_hand(card, &game->hand_init);
          
          to_remove[i-3] = card;
     }
     qsort(to_remove, rem_num, sizeof(Card), card_cmp);
     // to_remove is only needed for deck initialization, we can free it right after
     init_deck(&game->deck, RAZZ_CARDS, RAZZ_REP, to_remove, INITIAL_CARDS(num_players));
     free(to_remove);
}

void
usage() {
     fprintf(stderr, "Usage: ./razz <k> <c1_1> <c1_2> <c1_3> <c2_1> .. <cn_1>\n");
     fprintf(stderr, "\tk : exponent for number of simulations (10^k)\n");
     fprintf(stderr, "\tcx_y : yth card of xth player, player 1 wants 3 cards, other players only one.\n");
     fprintf(stderr, "\tfor example\n\t ./razz 6 2 A 2 3 4\n\twill run 1 million simulations with 2 players\n");
     fprintf(stderr, "\twith Ace,2,3 for player 1 and 4 for player 2\n");
     fprintf(stderr, "\tthe returned statistics represent the number of times for each ranking obtained by player 1\n");
     exit(EX_USAGE);
}
