#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int longestWordLength = 0;

typedef struct {
	unsigned int wordCount;
	unsigned int longestWordLength;
} DictInfo;

DictInfo getDictionaryInfo(char* path) {
	FILE* file = fopen(path, "r");
	DictInfo dictInfo;
	dictInfo.wordCount = 0;
	dictInfo.longestWordLength = 0;
	unsigned int currentWordLength = 0;
	
	if (file == NULL)
		perror("Error opening file");
	else
		while (! feof(file)) {
			if (fgetc(file) == '\n') {
				dictInfo.wordCount++;
				if (currentWordLength > dictInfo.longestWordLength)
					dictInfo.longestWordLength = currentWordLength;
				currentWordLength = 0;
			} else
				currentWordLength++;
		}
	fclose(file);
	
	return dictInfo;
}

void getWord(char* word, int index, char* path) {
	unsigned int count = 0;
	FILE* file = fopen(path, "r");
	while (! feof(file) && count < index) {
		count++;
		fgets(word, longestWordLength, file);
	}
	fclose(file);
	
	/* Assumes a new line character at the end of the string, and removes it */
	word[strlen(word) - 1] = '\0';
}

int main (int argc, const char* argv[]) {
	
	// This path differs on non Mac operating systems
	char* wordsPath = "/usr/share/dict/words";
	
	DictInfo dictInfo = getDictionaryInfo(wordsPath);
	longestWordLength = dictInfo.longestWordLength;
	
	srand(time(NULL));
	
	int wordIndex;
	do {
		wordIndex = rand();
	} while (wordIndex >= dictInfo.wordCount);
	
	char word[dictInfo.longestWordLength];
	getWord(word, wordIndex, wordsPath);
	
	printf("Word: %s, Word Index: %d, Total Words: %d", word, wordIndex, dictInfo.wordCount);
	
    return 0;
}
