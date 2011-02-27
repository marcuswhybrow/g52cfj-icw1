#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct {
	unsigned int wordCount, longestWordLength, acceptableWordCount, longestAcceptableWordLength;
} DictInfo;

typedef struct {
	char* word;
	char* guesses;
	char* finalState;
	char* remainingLetters;
	struct RoundResultItem* nextResult;
} RoundResultItem;

typedef struct {
	RoundResultItem* head;
	RoundResultItem* tail;
	unsigned int length;
} RoundResults;

char* wordsPath;
unsigned int longestWordLength = 0;
unsigned int wordCount = 0;
unsigned int longestAcceptableWordLength = 0;
unsigned int acceptableWordCount = 0;
int allowCaps = FALSE;
int maxWrongGuesses = 10;
RoundResults results = {NULL, NULL, 0};

char* putOnHeap(char* str) {
	char* heapStr = calloc(strlen(str)+1, sizeof(char));
	strcpy(heapStr, str);
	return heapStr;
}

void addResult(char* word, char* guesses, char* finalState, char* remainingLetters) {
	RoundResultItem* item = malloc(sizeof(RoundResultItem));
	item->word = putOnHeap(word);
	item->guesses = (char*)putOnHeap(guesses);
	item->finalState = (char*)putOnHeap(finalState);
	item->remainingLetters = (char*)putOnHeap(remainingLetters);
	
	if (results.length == 0)
		results.tail = item;
	else
		results.head->nextResult = (struct RoundResultItem*)item;
	results.head = item;
	results.length++;
}

int isAcceptableWord(char* word) {
	unsigned int i = strlen(word);
	char c;
	
	while (i--) {
		c = word[i];
		if (! (c >= 'a' && c <= 'z' || allowCaps && (word[0] >= 'A' && word[0] <= 'Z') && c >= 'A' && c <= 'Z'))
			return FALSE;
	}
	return TRUE;
}

DictInfo getDictionaryInfo(char* path) {
	FILE* file = fopen(path, "r");
	DictInfo dictInfo = {0,0,0,0};
	char word[1024];
	unsigned short length;
	
	if (file == NULL) {
		perror("Error opening file");
		exit(1);
	} else
		for (; ! feof(file); dictInfo.wordCount++) {
			fscanf(file, "%s\n", word);
			length = strlen(word);
			
			/* Update the acceptable word counts */
			if (isAcceptableWord(word)) {
				dictInfo.acceptableWordCount++;
				if (length > dictInfo.longestAcceptableWordLength)
					dictInfo.longestAcceptableWordLength = length;
			}
			
			/* Update the total word counts */
			if (length > dictInfo.longestWordLength)
				dictInfo.longestWordLength = length;
		}
	fclose(file);
	
	return dictInfo;
}

char* getWord(int index, char* path) {
	char word[longestWordLength];
	FILE* file = fopen(path, "r");
	for (unsigned int count = 0; ! feof(file) && count < index; count++)
		fscanf(file, "%s\n", word);
	fclose(file);
	char* w = calloc(strlen(word)+1, sizeof(char));
	strcpy(w, word);
	return w;
}

int areEqual(char c1, char c2) {
	if (c1 <= 'Z') c1 += 32;
	if (c2 <= 'Z') c2 += 32;
	
	return c1 == c2;
}

int hasUdiscoveredChar(char c, char* guessedWord, char* word) {
	for (unsigned short i = 0; i < strlen(guessedWord); i++)
		if (areEqual(word[i], c) && guessedWord[i] == '-')
			return i;
	return -1;
}

void playRound(char* word) {
	char input[1024];
	unsigned short guesses = maxWrongGuesses;
	char guessedWord[strlen(word)];
	char guessedSoFar[strlen(word)+guesses];
	char letters[27];
	unsigned short lettersRemaining = strlen(word);
	int charIndex;
	char guessedWordChar;
	char guess[2] = {'\0', '\0'};
	RoundResultItem* currentItem;
	char* path;
	FILE* file;
	
	strcpy(guessedWord, word);
	strcpy(guessedSoFar, "");
	
	/* Populate the available letters array */
	for (char c = 'A'; (int)c <= (int)'Z'; c++)
		letters[c - 'A'] = c;
	letters[26] = '\0';
	
	/* Fill the guessedWord with '-' in order to hide the letters */
	for (unsigned short i = 0; i < strlen(guessedWord); i++)
		guessedWord[i] = '-';
	
	while (TRUE) {
		printf("\n-\n\n");
		printf("Guess the word : '%s'\n", guessedWord);
		printf("Available      : %s\n", letters);
		printf("Guessed so far : %s\n", guessedSoFar);
		printf("%d more wrong guesses permitted.\n", guesses);
		printf("Type the letter of your guess and ENTER/RETURN: ");
		fgets(input, 100, stdin);
		
		switch (input[0]) {
			case '!':
				/* Exit the program */
				exit(0);
				break;
			case '?':
				/* Cheat. Display the current secret word to the user */
				printf("The word (you cheater) is: %s\n", word);
				break;
			case '*':
				/* Display the words that have been used so far, along with the guesses */
				currentItem = results.tail;
				for (int i = 0; i < results.length; i++) {
					printf("\nWord number : %d\n", i+1);
					printf("Word        : '%s'\n", currentItem->word);
					printf("Final guess : '%s'\n", currentItem->finalState);
					printf("Remaining   : '%s'\n", currentItem->remainingLetters);
					printf("Guesses     : '%s'\n", currentItem->guesses);
					currentItem = (RoundResultItem*)currentItem->nextResult;
				}
				break;
			case '$':
				/* Save the current words and guesses to a ﬁle */
				printf("\nSpecify a file to write the game history to:\n");
				
				/* Get the path from the user */
				fgets(input, 1024, stdin);
				
				/* Allocate a more appropriately sized bit of memory */
				path = calloc(strlen(input), sizeof(char));
				strcpy(path, input);
				
				/* Remove new line character from input */
				path[strlen(path)-1] = '\0';
				
				/* Ask for confirmation */
				printf("Are you sure you want to overwrite the file '%s' (y/N): ", path);
				fgets(input, 1024, stdin);
				switch (input[0]) {
					case 'y':
					case 'Y':
						/* Write to file */
						file = fopen(path, "w");
						currentItem = results.tail;
						for (int i = 0; i < results.length; i++) {
							fprintf(file, "Word %d: '%s', '%s', '%s', '%s'\n",
									i+1, currentItem->word, currentItem->finalState,
									currentItem->remainingLetters, currentItem->guesses);
							currentItem = (RoundResultItem*)currentItem->nextResult;
						}
						fclose(file);
						printf("Written to file '%s'!", path);
						break;
					default:
						/* Do not write to file */
						printf("Aborted, did NOT write to file!");
						break;
				}
				
				/* Deallocated the path memory allocated earlier */
				free(path);
				break;
			default:
				guess[0] = input[0];
				strcat(guessedSoFar, guess);
				
				charIndex = hasUdiscoveredChar(input[0], guessedWord, word);
				if (charIndex >= 0) {
					/* A correct guess */
					guessedWordChar = word[charIndex];
					
					/* Reveal the character that the player guessed correctly */
					guessedWord[charIndex] = guessedWordChar;
					
					/* Give the player information on their progression */
					printf("*** Yes! ’%c’ exists in the secret word. First match has been labelled.\n", input[0]);
					printf("*** You have %d letters still to guess.\n", --lettersRemaining);
					
					/* Remove the character form the available letters array if there are no more of this letter */
					if (hasUdiscoveredChar(guessedWordChar, guessedWord, word) < 0)
						letters[guessedWordChar - 'a'] = '-';
					
					/* The player has won if there are no more letters to be guessed */
					if (lettersRemaining <= 0) {
						printf("*** Well done, you have guessed all of the letters.\n");
						printf("\nWell done! The word was ’%s’.\n", word);
						addResult(word, guessedSoFar, guessedWord, letters);
						return;
					}
				} else {
					/* An incorrect guess */
					if (--guesses <= 0) {
						printf("\nUnfortunately you are out of guesses, the word was '%s'.\n", word);
						addResult(word, guessedSoFar, guessedWord, letters);
						return;
					} else
						printf("*** Wrong! You are permitted another %d wrong guesses before you lose.\n", guesses);
					
				}
		}
	}
}

unsigned int setSeed(unsigned int seed) {
	unsigned int s;
	if (seed == -1) {
		s = (unsigned int)time(NULL);
		srand(s);
		return s;
	} else {
		srand(seed);
		return seed;
	}

}

void setGlobals() {
	DictInfo di = getDictionaryInfo(wordsPath);
	longestWordLength = di.longestWordLength;
	wordCount = di.wordCount;
	longestAcceptableWordLength = di.longestAcceptableWordLength;
	acceptableWordCount = di.acceptableWordCount;
}

char* readRandomWord() {
	int wordIndex;
	char* word;
	do {
		wordIndex = rand();
	} while (wordIndex >= wordCount);
	
	return word = getWord(wordIndex, wordsPath);
}

void playGame() {
	char* word;
	
	while (TRUE) {
		/* Honours the allowCaps setting */
		do {
			word = readRandomWord();
		} while (! isAcceptableWord(word));
		
		playRound(word);
		free(word);
	}
}

int main (int argc, const char* argv[]) {
	unsigned int seed = -1;
	
	char arg[100];
	char flag;
	int i = argc;
	
	char path[1024];
	
	while (i--) {
		if (sscanf(argv[i], "-%c%s", &flag, arg) > 0) {
			switch (flag) {
				case 'g':
				case 'G':
					/* Number of wrong guesses */
					sscanf(arg, "%u", &maxWrongGuesses);
					break;
				case 's':
				case 'S':
					/* The seed */
					sscanf(arg, "%u", &seed);
					break;
				case 'f':
				case 'F':
					/* File path to the dictionary of words */
					sscanf(arg, "%s", path);
					wordsPath = calloc(strlen(path)+1, sizeof(char));
					strcpy(wordsPath, path);
					break;
				case 'n':
				case 'N':
					/* Allow real names (words that start with a capitol) */
					allowCaps = TRUE;
					break;
			}
		}
	}
	
	if (!wordsPath)
		wordsPath = "/usr/dict/words";
	
	printf("Welcome to CFJ Hangman (G52CFJ Coursework 1)\n");
	printf("-\n");
	
	seed = setSeed(seed);
	
	printf("Random seed was %d\n", seed);
	printf("Load dictionary from file: '%s'\n", wordsPath);

	/* Initialisation - loads dictionary */
	setGlobals();
	
	printf("%d words with max length %d\n", wordCount, longestWordLength);
	printf("%d acceptable words with max length %d\n", acceptableWordCount, longestAcceptableWordLength);
	printf("Real names %s allowed\n", allowCaps ? "ARE" : "are NOT");
	
	playGame();
	
    return 0;
}
