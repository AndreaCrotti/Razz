/**
 * @file   razz.h
 * @author Andrea Crotti <andrea.crotti.0@gmail.com>
 * @date   Wed Feb 24 17:38:32 2010
 * 
 * @brief  Interface for razz program
 * 
 * 
 */

#define RAZZ_HAND 7
#define RAZZ_EVAL 5
#define MAX_COUPLES (RAZZ_HAND - RAZZ_EVAL)
#define NON_HIGH_HAND (-1)
#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1
#define RAZZ_CARDS 13
#define RAZZ_REP 4
#define MIN_RANK 5
#define POSSIBLE_RANKS (RAZZ_CARDS - MIN_RANK + 2)

typedef short Card;
typedef struct Deck Deck;
typedef struct Hand Hand;
typedef struct Game Game;

/// A deck is basically an array of cards with an actual and original length
struct Deck {
     Card cards[RAZZ_CARDS * RAZZ_REP];
     int len;
     int orig_len;
};

struct Hand {
     Card cards[RAZZ_CARDS]; /**< dictionary idx -> occurrences */
     int len;
     int diffs;
};

struct Game
{
     Deck deck;
     int num_players;
     int num_simulations;
     Hand hand_init;
     long result[POSSIBLE_RANKS];
};

/** 
 * Swap two cards in the array at the given indices
 * 
 * @param int from position
 * @param int to position
 */
void swap_cards(int, int, Card *);

/** 
 * Initialize the deck
 * 
 * @param Deck * address of the deck to initialize
 * @param int number of cards of one subdeck
 * @param int number of repetitions of subdecks
 * @param Card * array of cards to remove while creating
 * @param int number of cards to remove
 */
void init_deck(Deck *, int, int, Card *, int);

/** 
 * Grab a random card from the deck and remove it
 * 
 * 
 * @return card randomly selected
 */
Card get_random_card_from_deck(Deck *);

/** 
 * Take cards from deck and add them to hand until it's full
 * 
 * @param Deck * source the cards to distribute
 * @param Hand * hand modified and ranked
 */
void fill_hand(Deck *, Hand *);

/** 
 * Main loop, runs many simulations setting the result as a side effect
 *
 * @param Game * game configuration
 */
void loop(Game *);

/** 
 * Initialize an empty hand to 0
 * @param Hand * hand address to inizialize
 */
void init_hand(Hand *);

/** 
 * add one card to the hand
 * 
 * @param Card card to add
 * @param Hand * address of the hand to modify
 */
void add_card_to_hand(Card, Hand *);

/** 
 * Core function, return the ranking of the hand
 * 
 * 
 * @return rank
 */
Card rank_hand(Hand *);

int char_to_card_idx(char);
int rank_to_result_idx(int);
int idx_to_rank(int);

void output_result(long *, long);
void usage(void);
void get_args(int, char **, Game *);

int card_cmp(const void *, const void *);
void qsort(void *, size_t, size_t, int (*compar)(const void *, const void *));
long random(void);
