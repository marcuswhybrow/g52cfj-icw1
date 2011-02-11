#include <stdio.h>
#include <string.h>

int main (int argc, const char* argv[]) {
	
	// This path differs on non Mac operating systems
	char* wordsPath = "/usr/share/dict/words";
	
	FILE* words = fopen(wordsPath, "r");
	unsigned int lineCount = 0;
	
	if (words == NULL)
		perror("Error opening file");
	else
		while (! feof(words))
			if (fgetc(words) == '\n')
				lineCount++;
	
	printf("%d\n", lineCount);

	
    return 0;
}
