#include <stdio.h>
#include "razz.h"

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
