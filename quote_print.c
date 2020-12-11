#include <stdio.h> /* For printf(), fopen(), fgets(), fclose() */
#include <stdlib.h> /* For malloc(), free(), srand(), rand(), exit() */
#include <time.h>  /* For time() */
#include <stdbool.h> /* For true */
#include <string.h> /* For strcpy() */

/*
 * Quote Print
 * Author: xraytp
 * 
 * Program which takes quotes and their authors from a filepath given at 
 * commandline execution. Randomly selects one, formats it for color and 80
 * character per line maximum and prints it.
 *
 * Error Handling:
 * 
 * Exit code 3: Errors openning quote file
 * Exit code 4: Errors closing quote file 
 * Exit code 6: Odd number of quotes in quote file, all quotes in quote file
 * must have an author assigned.
 * Exit code 8: Out of memory error
 */

/* Size of quote buffer in quoteSetList */
#define SIZEOFQUOTEBUFFER 16384 
/* Size of author buffer in quoteSetList */
#define SIZEOFAUTHORBUFFER 2048

/* Linked list of quotes and authors */
struct quoteSetList {
	char quote[SIZEOFQUOTEBUFFER];
	char author[SIZEOFAUTHORBUFFER];
	struct quoteSetList* nextSet;
};

char quoteFileFilepath[8192]; /* Filepath of quote file */

/* Populates linked list of a quote sets where a set is a quote with its
 * respective author. Returns error code.
 *
 * Error codes:
 * 3 - Errors openning quote file
 * 4 - Errors closing quote file
 * 6 - Odd number of lines in quotefile
 * 8 - Out of memory
 */
int populateQuoteSetList(
	char* filepath, /* filepath of quote file */
	struct quoteSetList** quotes, /* quoteSetList pointer to populate */
	int* numberOfQuotes /* number of quotes in populated list */)
{
	/* Open file of quotes */
	FILE* quoteFile; /* File of quotes */
	quoteFile = fopen(filepath, "r"); 
	if(quoteFile == NULL)
	{
		fprintf(stderr, "Error openning file\n");
		return 3;
	}

	/* Read quote sets into quoteSetList items and populates list */
	/* Top of linked list */
	struct quoteSetList* topItem = NULL;
	for(*numberOfQuotes = 0;; ++(*numberOfQuotes))
	{
		struct quoteSetList* newItem;
		/* Prepare space for next item in linked list */
		newItem = (struct quoteSetList*)malloc(
			sizeof(struct quoteSetList));
		if(newItem == NULL)
		{
			fprintf(stderr, "Out of memory\n");
			return 8;
		}

		if(fgets(newItem->quote, SIZEOFQUOTEBUFFER, quoteFile) == NULL)
			break; /* No more quotes */
		if(fgets(newItem->author, SIZEOFAUTHORBUFFER, quoteFile)
			== NULL)
			return 6; 
			/* Odd number of lines in quote file, there is a quote
			 * without an author */

		/* Link to topItem and reassign topItem */
		newItem->nextSet = topItem;
		topItem = newItem;
	}
	
	/* Return created list */
	*quotes = topItem;

	/* Close file of quotes */
	if(fclose(quoteFile) != 0)
	{
		fprintf(stderr, "Quote file closed with errors\n");
		return 4;
	}

	return 0;
}
		
/* Frees items in a quoteSetList linked list */
void freeQuoteSetList(struct quoteSetList* list /* List to be freed */)
{
	/* Item to be immediately freed */
	struct quoteSetList* toBeFreed = list; 
	while(true)
	{
		if(toBeFreed == NULL)
			break; /* All items freed, list is empty */

		/* Note next item to be freed */
		struct quoteSetList* nextToBeFreed = toBeFreed->nextSet;

		free(toBeFreed);

		/* Reassign toBeFreed */
		toBeFreed = nextToBeFreed;
	}
}

/* Substitutes the last whitespace for a newline character where the string 
 * extends more than 80 characters since last newline character. 
 *
 * Limitations:
 * Does not add a newline character where a word is longer than 80 characters.
 */
void format80charnewline(char* string /* string to be formatted */)
{
	int iterator; /* For iteration through string */
	int posLastNewline; /* Position of last recorded newline character */
	/* Position of last recorded whitespace character */
	int posLastWhitespace; 
	for(
		iterator = 0, posLastNewline = 0, posLastWhitespace = 0; 
 		string[iterator] != '\0'; 
		iterator++)
	{
		switch(string[iterator])
		{
			case ' ':
			posLastWhitespace = iterator;
			break;
			case '\t': /* Tab character */
			posLastWhitespace = iterator;
			posLastNewline += 7; /* To account for size of tab */
			break;
			case '\n':
			posLastNewline = iterator;
		}
		
		/* If last newline character is more than 80 characters of
		 * difference from iterator, substitute the last recorded 
		 * whitespace with a newline character and update recorded 
		 * last newline character position. */
		if(iterator - posLastNewline > 80)
		{
			string[posLastWhitespace] = '\n';
			posLastNewline = posLastWhitespace;
		}
	}
}

int main(int argc, char** argv)
{
	/* Parse command line */
	int counter; /* Counter for following for loop */
	for(counter = 0; counter < argc; ++counter)
	{
		switch(argv[counter][1])
		{
			/* Obtain filepath of quote file */
			case 'f':
			strcpy(quoteFileFilepath, argv[counter+1]);
		}
	}

	/* Populate quote set list */
	struct quoteSetList* quotes;
	struct quoteSetList** quotesPtr = &quotes; /* Quote set linked list */
	int numberOfQuotes; /* Number of quote sets in quote set linked list */
	int* numberOfQuotesPtr = &numberOfQuotes;
	int populateReturn; /* populateQuoteSetList returned value */
	populateReturn = 
		populateQuoteSetList(
			quoteFileFilepath, 
			quotesPtr, 
			numberOfQuotesPtr);
	
	/* If populateQuoteSetList returned nonzero, exit with returned value 
	 */
	if(populateReturn)
		exit(populateReturn);

	/* Randomly select quote set */
	srand(time(NULL)); /* Seed randomness with current time */
	/* Quote set to be printed */
	struct quoteSetList* quoteSetToPrint = quotes; 
	/* Position of quote set to be printed in quote set list*/
	int quoteSetToPrintPos = rand() % numberOfQuotes; 
	/* Iterator for quote set list in following for loop */
	int quoteSetListIterator; 
	for(
		quoteSetListIterator = 0; 
		quoteSetListIterator < quoteSetToPrintPos;
		++quoteSetListIterator)
		quoteSetToPrint = quoteSetToPrint->nextSet;
	
	/* Format quote set to print and print*/
	format80charnewline(quoteSetToPrint->quote);
	format80charnewline(quoteSetToPrint->author);
	
	/* Add colour to quote */
	printf("\033[32;1m");
	/* Print quote */
	printf(quoteSetToPrint->quote);
	/* Add colour to author */
	printf("\033[36;1m");
	/* Print author */
	printf(quoteSetToPrint->author);
	/* Reset colour to white */
	printf("\033[0;37m");

	/* Free quote set list */
	freeQuoteSetList(quotes);

	return 0;
}
