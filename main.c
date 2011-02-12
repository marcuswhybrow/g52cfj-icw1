#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int longestWordLength = 0;

typedef struct {
	unsigned int wordCount, longestWordLength;
} DictInfo;

DictInfo getDictionaryInfo(char* path) {
	FILE* file = fopen(path, "r");
	DictInfo dictInfo = {0,0};
	char word[longestWordLength];
	unsigned short length;
	
	if (file == NULL)
		perror("Error opening file");
	else
		for (dictInfo.wordCount; ! feof(file); dictInfo.wordCount++) {
			fscanf(file, "%s\n", word);
			length = strlen(word);
			if (length > dictInfo.longestWordLength)
				dictInfo.longestWordLength = length;
		}
	fclose(file);
	
	return dictInfo;
}

void getWord(char* word, int index, char* path) {
	FILE* file = fopen(path, "r");
	for (unsigned int count = 0; ! feof(file) && count < index; count++)
		fscanf(file, "%s\n", word);
	fclose(file);
}

int hasUdiscoveredChar(char c, char* guessedWord, char* word) {
	for (unsigned short i = 0; i < strlen(guessedWord); i++)
		if (word[i] == c && guessedWord[i] == '-')
			return i;
	return -1;
}

void play(char* word) {
	char input[100];
	unsigned short guesses = 10;
	char guessedWord[strlen(word)];
	char letters[27];
	unsigned short lettersRemaining = strlen(word);
	int charIndex;
	
	strcpy(guessedWord, word);
	
	/* Populate the available letters array */
	for (char c = 'A'; (int)c <= (int)'Z'; c++)
		letters[(int)c - (int)'A'] = c;
	letters[26] = '\0';
	
	/* Fill the guessedWord with '-' in order to hide the letters */
	for (unsigned short i = 0; i < strlen(guessedWord); i++)
		guessedWord[i] = '-';
	
	while (TRUE) {
		printf("\n-\n\n");
		printf("Guess the word : '%s'\n", guessedWord);
		printf("Available      : %s\n", letters);
		printf("%d more wrong guesses permitted.\n", guesses);
		printf("%s\n", word);
		printf("Type the letter of your guess and ENTER/RETURN: ");
		fgets(input, 100, stdin);
		
		charIndex = hasUdiscoveredChar(input[0], guessedWord, word);
		if (charIndex >= 0) {
			/* A correct guess */
			
			/* Reveal the character that the player guessed correctly */
			guessedWord[charIndex] = word[charIndex];
			
			/* Give the player information on their progression */
			printf("*** Yes! ’%c’ exists in the secret word. First match has been labelled.\n", input[0]);
			printf("*** You have %d letters still to guess.\n", --lettersRemaining);
			
			/* Remove the character form the available letters array if there are no more of this letter */
			if (hasUdiscoveredChar(input[0], guessedWord, word) < 0)
				letters[(int)input[0] - (int)'a'] = '-';
			
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

int main (int argc, const char* argv[]) {
	
	// This path differs on non Mac operating systems
	char* wordsPath = "/usr/share/dict/words";
	unsigned int seed;
	
	DictInfo dictInfo = getDictionaryInfo(wordsPath);
	longestWordLength = dictInfo.longestWordLength;
	
	seed = (unsigned int)time(NULL);
	srand(seed);
	
	int wordIndex;
	do {
		wordIndex = rand();
	} while (wordIndex >= dictInfo.wordCount);
	
	char word[dictInfo.longestWordLength];
	getWord(word, wordIndex, wordsPath);
	
	printf("Welcome to CFJ Hangman (G52CFJ Coursework 1)\n");
	printf("-\n");
	printf("Random seed was %d\n", seed);
	printf("Load dictionary from file: '%s'\n", wordsPath);
	printf("%d words with max length %d\n", dictInfo.wordCount, dictInfo.longestWordLength);
	
	play(word);
	
    return 0;
}
