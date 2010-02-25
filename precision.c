#include <stdio.h>
#include <stdlib.h>
#include "razz.h"

#define TO_CARD(x) (x % RAZZ_CARDS)

#define K 7

Hand hand;
long result[POSSIBLE_RANKS];
int combination[K];

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

     for (i = 0; i < K; i++)
          combination[i] = i;

     int count = 0;
     int rank;
     // I could do something like:
     // - start from a triple
     // - until we are there keep the results
     // - print out the resulting probabilities for every initial triple
     // not so easy because this generation is not lexicographically ordered
     while (next_comb(combination, K, n)) {
          // initialize the hand and then start rewriting in it
          init_hand(&hand);
          // initialize to the same value
          // setting upper values to the new combination
          for (i = 0; i < K; i++) {
               add_card_to_hand(TO_CARD(combination[i]), &hand);
               printf("%d,", TO_CARD(combination[i])+1);
          }
          rank = rank_hand(&hand);
          result[rank_to_result_idx(rank)]++;
          count++;
          printf("%d\n", rank);
     }
     /* output_result(result, count); */
     /* printf("created %d combinations\n", count); */
     return 0;
}
