/**
 * @file   razz.h
 * @author Andrea Crotti <andrea.crotti.0@gmail.com>
 * @date   Fri Feb  5 21:10:41 2010
 * 
 * @brief  Razz simulation
 * 
 * 
 * 
 */


#define N_SIM (1000 * 10000)
#define RAZZ_HAND 7
#define RAZZ_EVAL 5
#define MAX_COUPLES (RAZZ_HAND - RAZZ_EVAL)

#define NON_HIGH_HAND (-1)

#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1

#define RAZZ_CARDS 13
#define RAZZ_REP 4

/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

// A card is just an integer
typedef int card;
typedef char rem;

typedef struct deck {
  card *cards;
  int len;
  int orig_len;
} deck;

typedef struct hand {
  card cards[RAZZ_CARDS]; /**< dictionary idx -> occurrences */
  int len;
  int rank;
  int num_couples;
} hand;

void swap_cards(int, int, card *);

deck *make_deck(const int, const int, const int);
void print_deck(deck *);
void free_deck(deck *);
card get_random_card_from_deck(deck *);
void remove_card_from_deck(card, deck *);
void remove_hand_from_deck(hand *, deck *);

void test_random_card();
void test_hand_ranking();
card play(deck *, int, hand *);
void loop();

hand *make_hand();
void add_card_to_hand(card, hand *);
void print_hand(hand *);
int hand_is_full(hand *);
void free_hand(hand *);
card rank_hand(hand *);

card char_to_card(char);
