#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

char* wordsPath;
unsigned int longestWordLength = 0;
unsigned int wordCount = 0;
unsigned int longestAcceptableWordLength = 0;
unsigned int acceptableWordCount = 0;
int allowCaps = FALSE;
int maxWrongGuesses = 10;

typedef struct {
	unsigned int wordCount, longestWordLength, acceptableWordCount, longestAcceptableWordLength;
} DictInfo;

int isAcceptableWord(char* word) {
	unsigned int i = strlen(word);
	char c;
	
	while (i--) {
		c = word[i];
		if (! (c >= 'a' && c <= 'z' || i == 0 && allowCaps && c >= 'A' && c <= 'Z')) {
			printf("Unacceptable Word: %s\n", word);
			return FALSE;
		}
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
	char input[100];
	unsigned short guesses = maxWrongGuesses;
	char guessedWord[strlen(word)];
	char guessedSoFar[strlen(word)+guesses];
	char letters[27];
	unsigned short lettersRemaining = strlen(word);
	int charIndex;
	char guessedWordChar;
	
	strcpy(guessedWord, word);
	
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
		printf("%s\n", word);
		printf("Type the letter of your guess and ENTER/RETURN: ");
		fgets(input, 100, stdin);
		
		char guess[2] = {input[0], '\0'};
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
				break;
			}
		} else {
			/* An incorrect guess */
			if (--guesses <= 0) {
				printf("\nUnfortunately you are out of guesses, the word was '%s'.\n", word);
				break;
			} else
				printf("*** Wrong! You are permitted another %d wrong guesses before you lose.\n", guesses);
			
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
