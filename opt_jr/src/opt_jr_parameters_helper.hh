#ifndef HELPER_HH
#define HELPER_HH


/*
GLOBAL VARIABLES USEFUL TO CREATE OBJECT OF CLASS OPTJRPARAMETER
*/

#define ARGS 9 ///< number of arguments from command line

/* COMMAND LINE OPTIONS */
#define FILENAME "-f="
#define NUM_N "-n="
#define LIST_LIMIT "-k="
#define DEBUG "-d="
#define MAX_ITERATIONS "-i="
#define GLOBAL_FO_CALCULATION "-g"
#define CACHE "-c"
#define SEARCH_TYPE "-st="

/* USEFUL GLOBAL VARIABLES to parse args*/
#define NUMBER 0
#define STRING 1
#define YES_NO 2

#define NO 0
#define YES 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    A FEW OPTJRPARAMETERS HELPER FUNCTION
*/



/**
Usage explains the usage of the OPT_JR_CPP program.
*/
void Usage(int argc);

/**
Function to parse argument from command line;
Invoked by Opt_jr_parameters constructor
*/
char* parseArg(char * string, char * gap, int type, int argc);




#endif
