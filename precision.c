// -*- compile-command: "gcc -Wall -o precision precision.c razz_impl.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include "razz.h"

#define TO_CARD(x) (x % RAZZ_CARDS)

Hand hand;
long result[POSSIBLE_RANKS];

/* Prints out a combination like {1, 2} */
void printc(int comb[], int k) {
     printf("{");
     int i;
     for (i = 0; i < k; ++i)
          printf("%d, ", TO_CARD(comb[i]));
     printf("\b\b}\n");
}

// FIXME: try to get from 0->51 instead of 1->52
int next_comb(int comb[], int k, int n) {
     int i = k - 1;
     ++comb[i];
     // check for "overflow" and increment backward all what you find
     while ((i >= 0) && (comb[i] >= n - k + 1 + i)) {
          --i;
          ++comb[i];
     }
     if (comb[0] > n - k) /* Combination (n-k, n-k+1, ..., n) reached */
          return 0; /* No more combinations can be generated */

     /* comb now looks like (..., x, n, n, n, ..., n).
	Turn it into (..., x, x + 1, x + 2, ...) */
     for (i = i + 1; i < k; ++i)
          comb[i] = comb[i - 1] + 1;

     return 1;
}

int main(int argc, char *argv[]) {
     int i;
     int n = 52;
     int k = 4;
     int *combination = malloc(sizeof(int) * k);

     for (i = 0; i < k; i++)
          combination[i] = i;

     Card *inits = malloc(sizeof(int) * 3);
     inits[0] = 0;
     inits[1] = 1;
     inits[2] = 2;
     
     // remove the 3 initial inits from the deck and then at every step
     // create a hand and remove it
     /* init_deck(&deck, RAZZ_CARDS, RAZZ_REP, inits, 3); */
     // deck is actually not really needed unless the combination loops through it

     int count = 0;
     int rank;
     // removing cards is not so easy
     while (next_comb(combination, k, n)) {
          // initialize the hand and then start rewriting in it
          init_hand(&hand);
          // initialize to the same value
          for (i = 0; i < 3; i++) {
               add_card_to_hand(inits[i], &hand);
          }
          // setting upper values to the new combination
          for (i = 0; i < 4; i++) {
               add_card_to_hand(TO_CARD(combination[i]), &hand);
          }
          rank = rank_hand(&hand);
          result[rank_to_result_idx(rank)]++;
          /* printc(combination, k); */
          /* printf("rank obtained = %d\n", rank); */
          count++;
     }
     output_result(result, count);
     printf("created %d combinations\n", count);
     free(combination);
     free(inits);
     return 0;
}
