#define RAZZ_HAND 7
#define RAZZ_EVAL 5
#define MAX_COUPLES (RAZZ_HAND - RAZZ_EVAL)

#define NON_HIGH_HAND (-1)

#define INITIAL_PLAYER 3
#define INITIAL_OTHER 1
#define INITIAL_CARDS(x) (INITIAL_PLAYER + (INITIAL_OTHER * (x - 1)))

#define RAZZ_CARDS 13
#define RAZZ_REP 4

#define MIN_RANK 5

#define POSSIBLE_RANKS (RAZZ_CARDS - MIN_RANK + 2)

/// those two macro makes the mapping index->card for the pseudo dictionary hand structure
#define CARD_TO_IDX(x) (x - 1)
#define IDX_TO_CARD(x) (x + 1)

// A card is just an integer
typedef int Card;
typedef struct Deck Deck;
typedef struct Hand Hand;

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

void swap_cards(int, int, Card *);

void init_deck(Deck *, int, int, int, Card *, int);
Card get_random_card_from_deck(Deck *);

Card play(Deck *, int, Hand *);
void loop(Hand *, long *, Card *);

void init_hand(Hand *);
void add_card_to_hand(Card, Hand *);

Card rank_hand(Hand *);

int char_to_card_idx(char);
int rank_to_result_idx(int);
int idx_to_rank(int);

void output_result(long *);
void usage();
void check_args(int, char **);

int intcmp(const void *, const void *);
void qsort(void *, size_t, size_t, int (*compar)(const void *, const void *));
long lrand48(void);

