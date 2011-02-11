#include <stdio.h>
#include <string.h>

int main (int argc, const char* argv[]) {
	
	// This path differs on non Mac operating systems
	char* wordsPath = "/usr/share/dict/words";
	
	FILE* words = fopen(wordsPath, "r");
	unsigned int lineCount = 0;
	unsigned int longestWordLength = 0;
	unsigned int currentWordLength = 0;
	
	if (words == NULL)
		perror("Error opening file");
	else
		while (! feof(words)) {
			if (fgetc(words) == '\n') {
				lineCount++;
				if (currentWordLength > longestWordLength)
					longestWordLength = currentWordLength;
				currentWordLength = 0;
			} else
				currentWordLength++;
		}
	
	printf("word count: %d, longest word length: %d\n", lineCount, longestWordLength);
	
    return 0;
}
