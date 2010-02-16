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
     Card *to_remove;
     Hand hand_init;
     long result[POSSIBLE_RANKS];
};

void swap_cards(int, int, Card *);

void init_deck(Deck *, int, int, Card *, int);
Card get_random_card_from_deck(Deck *);

int give_and_rank(Deck *, Hand *);
void loop(Game *);

void init_hand(Hand *);
void add_card_to_hand(Card, Hand *);

Card rank_hand(Hand *);

int char_to_card_idx(char);
int rank_to_result_idx(int);
int idx_to_rank(int);

void output_result(long *);
void usage(void);
void get_args(int, char **, Game *);

int card_cmp(const void *, const void *);
void qsort(void *, size_t, size_t, int (*compar)(const void *, const void *));
long random(void);
