#ifndef INVOKEPREDICTOR_HELPER_HH
#define INVOKEPREDICTOR_HELPER_HH

#define BIG_LINE 4000
#define BIG_TEXT 20000

#include "opt_jr_parameters.hh"
#include "utility.hh"

#include <string.h>
#include <stdio.h>


/*
-Here are declared helper functions to invoke the predictor
*/




/**
readFolder is an helper function used by invoke predictor; it returns
the first subFolder in the folder corresponding to "path"
*/
char * readFolder(char *  path);


/**
_run executes a command ("cmd")
*/
char * _run(char * cmd, Opt_jr_parameters &par);



void writeFile(const char *filepath, const char *data);
char * ls(char * pattern, Opt_jr_parameters &par);
char * extractRowN(char *text, int row);
char * replace(char * text, char *newLine);
char * readFile(char * filename);
char * extractWord(char * line, int pos);
char * extractRowMatchingPattern(char *text, char *pattern);

/**
 * Name 		extractItem
 * Input 		const char *const string, const char *const left, const char *const right
 * Output 		char * A wor
 * Description:	It returns a word delimited between two strings. If the second string is NULL, it just returns the last item in the string
 */
char * extractItem(const char *const string, const char *const left, const char *const right);




#endif
