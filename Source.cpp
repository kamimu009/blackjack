#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define ENABLE_LOGD 1

typedef enum {
	ERROR_NONE,
	ERROR_INVALID_ARG,
	ERROR_NO_MEMORY,
	ERROR_EMPTY,
	ERROR_GAME_OVER,
	ERROR_MAX
} Error;

typedef enum {
	MARK_HEART,
	MARK_DIAMOND,
	MARK_CLUB,
	MARK_SPADE,
	MARK_MAX
} Mark;

#define TRUE (1 == 1)
#define FALSE (!TRUE)

#define MAX_CARD_NUMBER 13
#define DECK_NUM	 (13 * MARK_MAX)
#define TARGET_SCORE 21

#define SUCCESS(r) ((r) == ERROR_NONE)
#define FAILURE(r) (!SUCCESS(r))
#define NUN_OF_ELEMENTS(x) (sizeof((x)) / sizeof((x)[0]))

#if ENABLE_LOGD
#define LOGD(fmt, ...) (printf("%s(%d) [DEBUG]: " fmt "\n", __FUNCTION__, __LINE__, __VA_ARGS__))
#else
#define LOGD(fmt, ...)
#endif
#define LOGI(fmt, ...) (printf("%s(%d) [INFO]: " fmt "\n", __FUNCTION__, __LINE__, __VA_ARGS__))

typedef struct {
	int scores[2];
} CardMeta;

typedef struct {
	Mark mark;
	int num;
} Card;

typedef struct {
	Card cards[DECK_NUM];
} Cards;

typedef struct {
	int topIndex;
	int numCard;
	const Card* pCards[DECK_NUM];
} Deck;

typedef struct {
	int numCard;
	const Card* cards[DECK_NUM];
} Hand;

const CardMeta cardMeta[MAX_CARD_NUMBER] = {
	{ {  1, 11 } },
	{ {  2, -1 } },
	{ {  3, -1 } },
	{ {  4, -1 } },
	{ {  5, -1 } },
	{ {  6, -1 } },
	{ {  7, -1 } },
	{ {  8, -1 } },
	{ {  9, -1 } },
	{ { 10, -1 } },
	{ { 10, -1 } },
	{ { 10, -1 } },
	{ { 10, -1 } },
};

const char* markName[MARK_MAX] = {
	"Heart",
	"Diamond",
	"Club",
	"Spade",
};

const char* numberName[MAX_CARD_NUMBER] = {
	"A",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"Jack",
	"Queen",
	"King",
};


void printHand(const char* playerName, const Hand* pHand, bool isOnlyOne);
Error initCards(Cards* pCards);
Error initDeck(const Cards* cards, Deck* pDeck);
Error initHand(Hand* pHand);
Error shuffle(Deck* pDeck, int shuffleNum);
Error pullCard(Deck* pDeck, Hand* pHand);
int calcScore(Hand* pHand);
const char* getNumberName(int number);
const char* getMarkName(Mark mark);


int main() {
	Error r = ERROR_NONE;
	Cards cards;
	Deck deck;
	Hand playerHand;
	Hand dealerHand;
	if (SUCCESS(r)) {
		(void)initCards(&cards);
		(void)initDeck(&cards, &deck);
		(void)initDeck(&cards, &deck);
		(void)shuffle(&deck, 100);
	}
	if (SUCCESS(r)) {
		bool isEnd = FALSE;
		while (!isEnd) {
			if (SUCCESS(r)) {
				(void)initHand(&playerHand);
				(void)initHand(&dealerHand);
			}
			if (SUCCESS(r)) {
				for (int i = 0; i < 2; i++) {
					(void)pullCard(&deck, &playerHand);
					(void)pullCard(&deck, &dealerHand);
				}
			}
			if (SUCCESS(r)) {
				printf("Geme start.\n");
				bool isEnd = FALSE;
				char c;
				printHand("Dealer", &dealerHand, TRUE);
				while (!isEnd) {
					if (SUCCESS(r)) {
						printHand("You", &playerHand, FALSE);
						printf("Hit? y/N: ");
						c = getchar();
						while ((getchar()) != '\n');
						printf("\n");
						isEnd = (c != 'y' && c != 'Y');
						if (!isEnd) {
							r = pullCard(&deck, &playerHand);
						}
					}
					if (SUCCESS(r)) {
						if (calcScore(&playerHand) < 0) {
							printHand("You", &playerHand, FALSE);
							printf("Burst!!! You lose.\n");
							isEnd = TRUE;
							r = ERROR_GAME_OVER;
						}
					}
					if (FAILURE(r)) {
						break;
					}
				}
			}
			if (SUCCESS(r)) {
				while (0 < calcScore(&dealerHand) && calcScore(&dealerHand) < 17) {
					r = pullCard(&deck, &dealerHand);
					if (FAILURE(r)) {
						break;
					}
				}
			}
			if (SUCCESS(r)) {
				char playerScoreStr[32];
				char dealerScoreStr[32];
				snprintf(playerScoreStr, sizeof(playerScoreStr), "%d", calcScore(&playerHand));
				snprintf(dealerScoreStr, sizeof(dealerScoreStr), "%d", calcScore(&dealerHand));
				printHand("You", &playerHand, FALSE);
				printHand("Dealer", &dealerHand, FALSE);
				printf("You=%s : Dealer=%s\n",
					calcScore(&playerHand) < 0 ? "Burst" : playerScoreStr,
					calcScore(&dealerHand) < 0 ? "Burst" : dealerScoreStr);
				if (calcScore(&playerHand) > calcScore(&dealerHand)) {
					printf("You win.\n");
				}
				else if (calcScore(&playerHand) == calcScore(&dealerHand)) {
					printf("Draw.\n");
				}
				else {
					printf("You lose.\n");
				}
			}
			if (FAILURE(r)) {
				if (r != ERROR_GAME_OVER) {
					LOGD("probably bug.\n");
					break;
				} else {
					r = ERROR_NONE;
				}
			}
			if (SUCCESS(r)) {
				if (deck.numCard > 20) {
					char c;
					printf("Continue? y/N: ");
					c = getchar();
					while ((getchar()) != '\n');
					isEnd = (c != 'y' && c != 'Y');
					printf("\n");
				} else {
					printf("Maybe next time.\n");
					isEnd = TRUE;
					getchar();
				}
			}
		}
	}
}

void printHand(const char* playerName, const Hand* pHand, bool isOnlyOne) {
	printf("%s:\n", playerName);
	for (int i = 0; i < pHand->numCard; i++) {
		printf("  %s %s\n",
			getMarkName(pHand->cards[i]->mark),
			getNumberName(pHand->cards[i]->num));
		if (isOnlyOne) {
			break;
		}
	}
}

Error initCards(Cards* pCards) {
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pCards == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		for (int mark = 0; mark < MARK_MAX; mark++) {
			for (unsigned int i = 0; i < MAX_CARD_NUMBER; i++) {
				Card* pCard = &pCards->cards[mark* MAX_CARD_NUMBER + i];
				pCard->mark = (Mark)mark;
				pCard->num = i;
			}
		}
	}
	return r;
};

Error initDeck(const Cards* pCards, Deck* pDeck) {
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pCards == NULL || pDeck == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		pDeck->numCard = 0;
		pDeck->topIndex = 0;
		for (int i = 0; i < NUN_OF_ELEMENTS(pDeck->pCards); i++) {
			pDeck->pCards[i] = &pCards->cards[i];
			pDeck->numCard++;
		}
	}
	return r;
}

Error initHand(Hand* pHand) {
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pHand == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		pHand->numCard = 0;
		for (int i = 0; i < NUN_OF_ELEMENTS(pHand->cards); i++) {
			pHand->cards[i] = NULL;
		}
	}
	return r;
}

Error shuffle(Deck* pDeck, int shuffleNum) {
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pDeck == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		for (int i = pDeck->topIndex; i < pDeck->numCard; i++) {
			LOGD("[Before]i=%d, mark=%d, num=%d", i, pDeck->pCards[i]->mark, pDeck->pCards[i]->num);
		}
	}
	if (SUCCESS(r)) {
		time_t  t = time(NULL);
		srand((unsigned int)t);
		for (int i = 0; i < shuffleNum; i++) {
			int target = rand() % pDeck->numCard;
			const Card* pTop = pDeck->pCards[pDeck->topIndex];
			pDeck->pCards[0] = pDeck->pCards[target];
			pDeck->pCards[target] = pTop;
		}
	}
	if (SUCCESS(r)) {
		for (int i = pDeck->topIndex; i < pDeck->numCard; i++) {
			LOGD("[After]i=%d, mark=%d, num=%d", i, pDeck->pCards[i]->mark, pDeck->pCards[i]->num);
		}
	}
	return r;
}

Error pullCard(Deck* pDeck, Hand* pHand) {
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pDeck == NULL || pHand == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		if (pDeck->numCard > 0) {
			pHand->cards[pHand->numCard] = pDeck->pCards[pDeck->topIndex];
			pHand->numCard++;
			pDeck->topIndex++;
			pDeck->numCard--;
		}
		else {
			r = ERROR_EMPTY;
		}
	}
	return r;
}

int calcScore(Hand* pHand) {
	int highScore = -1;
	Error r = ERROR_NONE;
	if (SUCCESS(r)) {
		if (pHand == NULL) {
			r = ERROR_INVALID_ARG;
		}
	}
	if (SUCCESS(r)) {
		for (int i = 0; i < pHand->numCard; i++) {
			LOGD("i=%d, card={num=%s, mark=%s}", i, getNumberName(pHand->cards[i]->num), getMarkName(pHand->cards[i]->mark));
		}
		for (int i = 0; i < pHand->numCard; i++) {
			for (int j = 0; j < NUN_OF_ELEMENTS(cardMeta[0].scores); j++) {
				int score = 0;
				int pattern = (i * NUN_OF_ELEMENTS(cardMeta[0].scores)) + j;
				int scoreIndexArray[NUN_OF_ELEMENTS(pHand->cards)];
				int dec = pattern;
				for (int k = 0; k < pHand->numCard; k++) {
					scoreIndexArray[k] = dec % NUN_OF_ELEMENTS(cardMeta[0].scores);
					dec /= NUN_OF_ELEMENTS(cardMeta[0].scores);
					LOGD("scoreIndexArray[%d]=%d, dec=%d, pattern=%d", k, scoreIndexArray[k], dec, pattern);
				}
				for (int k = 0; k < pHand->numCard; k++) {
					score += cardMeta[pHand->cards[k]->num].scores[scoreIndexArray[k]] < 0 ?
						cardMeta[pHand->cards[k]->num].scores[0] :
						cardMeta[pHand->cards[k]->num].scores[scoreIndexArray[k]];
				}
				highScore = highScore > score ? highScore : score > TARGET_SCORE ? highScore : score;
				LOGD("highScore=%d, score=%d", highScore, score);
			}
		}
	}
	return highScore;
}

const char* getNumberName(int number) {
	const char* name = NULL;
	if (number < NUN_OF_ELEMENTS(numberName)) {
		name = numberName[number];
	}
	return name;
}

const char* getMarkName(Mark mark) {
	const char* name = NULL;
	if (mark < NUN_OF_ELEMENTS(markName)) {
		name = markName[mark];
	}
	return name;
}
