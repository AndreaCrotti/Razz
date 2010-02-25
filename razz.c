#include <stdio.h>
#include "razz.h"

/*
  - make it multithreading, see http://softpixel.com/~cwright/programming/threads/threads.c.php
  - use couples instead of diffs for a better usage
  - adding another structure Simulation would be better?
*/

Game game_conf;

int main(int argc, char *argv[])
{
     srandom(time(0));
     init_hand(&game_conf.hand_init);
     get_args(argc, argv, &game_conf);
     
     loop(&game_conf);
     output_result(game_conf.result, game_conf.num_simulations);
     return 0;
}
