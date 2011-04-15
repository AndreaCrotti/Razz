#define _XOPEN_SOURCE 500

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <time.h>

#include "razz.h"

// internally we always use indices for cards, the first macro is only for output
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

#define RAZZ_HAND 7
#define RAZZ_EVAL 5
#define NON_HIGH_HAND (-1)
#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1

#define TO_EXP(x) powl(10, (x))
#define INITIAL_CARDS(x) (INITIAL_PLAYER + (INITIAL_OTHER * (x - 1)))

Game game_conf;

void fill_hand(Deck *deck, Hand *hand)
{
    while (hand->len < RAZZ_HAND) {
        add_card_to_hand(get_random_card_from_deck(deck), hand);
    }
}

void init_hand(Hand *hand)
{
    hand->len = 0;
    hand->diffs = 0;
    memset(hand->cards, 0, sizeof(Card) * RAZZ_CARDS);
}

void add_card_to_hand(Card c, Hand *hand)
{
    if (!hand->cards[c])
        hand->diffs++;

    hand->cards[c]++;
    hand->len++;
}

Card rank_hand(Hand *hand)
{
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
void init_deck(Deck *deck, int len_subdeck, int rep, Card cards_to_remove[], int to_remove)
{
    Card i;
    int j, idx, rem_idx;
    idx = rem_idx = 0;

    int len = (len_subdeck * rep) - to_remove;
    deck->len = deck->orig_len = len;
  
    for (i = 0; i < len_subdeck; i++) {
        for (j = 0; j < rep; ) {
            while ((i == cards_to_remove[rem_idx]) && (rem_idx < to_remove)) {
                assert(j < rep);
                j++;
                rem_idx++;
            }
            // if still we have other i cards we can add them to the deck
            if (j++ < rep)
                deck->cards[idx++] = i;
        }
    }
}

Card get_random_card_from_deck(Deck *deck)
{
    int pos = (int) (deck->len * (random() / (RAND_MAX + 1.0)));
    Card card = deck->cards[pos];
    // swap found element with the last one and then shrink the deck
    swap_cards(pos, deck->len-1, deck->cards);
    deck->len--;
    return card;
}

void swap_cards(int c1_idx, int c2_idx, Card *cards)
{
    Card tmp;
    tmp = cards[c1_idx];
    cards[c1_idx] = cards[c2_idx];
    cards[c2_idx] = tmp;
}

int rank_to_result_idx(int rank)
{
    switch (rank) {
    case -1: return 0;
    default : return (rank - MIN_RANK + 1);
    }
}

int idx_to_rank(int idx)
{
    switch (idx) {
    case 0: return -1;
    default : return (idx + MIN_RANK -1);
    }
}

int char_to_card_idx(char c)
{
    switch ( c ) {
    case 'A': return 0;
    case 'J': return 10;
    case 'Q': return 11;
    case 'K': return 12;
    case '1' : return 9; // dirty trick, make sure we NEVER get 1 instead of A
    default : return CARD_TO_IDX(c - '0');
    }
}

void output_result(long *result, long num_simulations)
{
    int i;
    double res;
    for (i = 0; i < POSSIBLE_RANKS; i++) {
        res = ((double) result[i] / num_simulations);
        if (res != 0)
            printf("%d:\t%f\n", idx_to_rank(i), res);
    }
}

int card_cmp(const void *v1, const void *v2)
{
    return (*(Card *)v1 - *(Card *)v2);
}

void loop(Game *game)
{
    int i, rank;

    // We use only ONE deck! already initialized without the initial hand
    Deck *deck = &game->deck;
    Hand hand_tmp;

    for (i = 0; i < game->num_simulations; i++) {
        // restore initial deck len, without really shuffling or creating a new one
        deck->len = deck->orig_len;
        // restore the hand to the initial state at every loop
        hand_tmp = game->hand_init;
        fill_hand(deck, &hand_tmp);

        rank = rank_hand(&hand_tmp);
        assert(rank != 0);
        game->result[rank_to_result_idx(rank)]++;
    }
}

void usage()
{
    fprintf(stderr, "Usage: ./razz <k> <c1_1> <c1_2> <c1_3> <c2_1> .. <cn_1>\n");
    fprintf(stderr, "\tk : exponent for number of simulations (10^k)\n");
    fprintf(stderr, "\tcx_y : yth card of xth player, player 1 wants 3 cards, other players only one.\n");
    fprintf(stderr, "\tfor example\n\t ./razz 6 A 2 3 4\n\twill run 1 million simulations with 2 players\n");
    fprintf(stderr, "\twith Ace,2,3 for player 1 and 4 for player 2\n");
    fprintf(stderr, "\tthe returned statistics represent the probabilities of each ranking obtained by player 1\n");
    exit(EX_USAGE);
}

void get_args(int argc, char *argv[], Game *game)
{
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
          
        to_remove[i-2] = card;
    }
    qsort(to_remove, rem_num, sizeof(Card), card_cmp);
    // to_remove is only needed for deck initialization, we can free it right after
    init_deck(&game->deck, RAZZ_CARDS, RAZZ_REP, to_remove, INITIAL_CARDS(num_players));
    free(to_remove);
}

int main(int argc, char *argv[])
{
    srandom(time(0));
    init_hand(&game_conf.hand_init);
    get_args(argc, argv, &game_conf);
     
    loop(&game_conf);
    output_result(game_conf.result, game_conf.num_simulations);
    return 0;
}
