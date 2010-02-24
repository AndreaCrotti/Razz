// -*- compile-command: "gcc -Wall -o precision precision.c razz_impl.c" -*-
#include <stdio.h>
#include <stdlib.h>
#include "razz.h"

/* Prints out a combination like {1, 2} */
void printc(int comb[], int k) {
     printf("{");
     int i;
     for (i = 0; i < k; ++i)
          printf("%d, ", comb[i] + 1);
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
     int k = 3;
     int comb2[3];
     for (i = 0; i < k; i++)
          comb2[i] = i;

     Card *inits = malloc(sizeof(int) * 3);
     inits[0] = 0;
     inits[1] = 1;
     inits[2] = 2;
     
     Card cards[7];
     // initialize to the same value
     for (i = 0; i < 3; i++) {
          cards[i] = inits[i];
     }

     Deck *deck;
     // remove the 3 initial inits from the deck and then at every step
     // create a hand and remove it
     init_deck(deck, RAZZ_CARDS, RAZZ_REP, inits, 3); // shorten this

     int count = 0;
     while (next_comb(comb2, k, n)) {
          // setting upper values to the new combination
          for (i = 0; i < 4; i++) {
               cards[3 + i] = comb2[i];
               // There is a coupling problem in give-and-rank!!
          }
          printc(comb2, k);
          count++;
     }
     printf("created %d combinations\n", count);
     return 0;
}
