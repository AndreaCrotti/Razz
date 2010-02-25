#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "razz.h"

/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

void
loop(Game *game) {
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

void
fill_hand(Deck *deck, Hand *hand) {
     while (hand->len < RAZZ_HAND) {
          add_card_to_hand(get_random_card_from_deck(deck), hand);
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
init_deck(Deck *deck, int len_subdeck, int rep, Card cards_to_remove[], int to_remove) {
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

Card
get_random_card_from_deck(Deck *deck) {
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
     case '1' : return 9; // dirty trick, make sure we NEVER get 1 instead of A
     default : return CARD_TO_IDX(c - '0');
     }
}

void
output_result(long *result, long num_simulations) {
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
