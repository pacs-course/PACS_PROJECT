#ifndef INVOKEPREDICTOR_HELPER_HH
#define INVOKEPREDICTOR_HELPER_HH

#define BIG_LINE 4000
#define BIG_TEXT 20000

#include "optjrparameters.hh"
#include "utility.hh"

#include <string.h>
#include <stdio.h>

/*
	-Here are declared helper functions to invoke a predictor
 */




 /**
  * 		Name:					readFolder
  * 		Input parameters:		A path to a folder
  * 		Output parameters:		The name of subfolder contained in the folder corresponding to the folder in "path"
  * 		Description:			It's an helper function used by invoke predictor; this function returns
  * 											the first subFolder in the folder corresponding to "path"
  */

char * readFolder(char *  path);




/**
 		Name:					_run
		Output parameters:		The output provided by the executed command
		Description:			 This function executes a command ("cmd")
 */
char * _run(char * cmd, OPT_JR_parameters &par);



void writeFile(const char *filepath, const char *data);
char * ls(char * pattern, OPT_JR_parameters &par);
char * extractRowN(char *text, int row);
char * replace(char * text, char *newLine);
char * readFile(char * filename);
char * extractWord(char * line, int pos);
char * extractRowMatchingPattern(char *text, char *pattern);



#endif
