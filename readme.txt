G52CFJ - C/C++ For Java Programmers - Information Coursework 1
Author: Marcus Whybrow
Deadline: 25th February 2011

This README file contains information for the markers of this informal 
coursework.

The -n flag
===========

A word is accept as a name if it has a capitol letter as it's first letter. 
From then on any character may be capitalised, for example

* USA
* McAllen
* Marcus

would all be considered names, and accepted as words.


Dictionary words in memory
==========================

Rather than loading all words into memory, an initial read of every line in 
the specified dictionary file discovers how many entries there are and records 
the length of the longest word found.

This information then allows the retrieval of a random word to be calculated 
more fairly. Only the current word is held in memory and the file is accessed 
again and to load in each new random word, this was judge to be more efficient 
at least in terms of memory.