#ifndef HELPER_HH
#define HELPER_HH


/************************************************************
VARIABILI GLOBALI UTILI PER CREARE OGGETTO DI CLASSE
OPTJRPARAMETER, CHE MEMORIZZA PARAMETRI PASSATI DA COMMAND_LINE
************************************************************/



#define ARGS 8 //number of arguments from command line

#define FILENAME "-f="
#define NUM_N "-n="
#define LIST_LIMIT "-k="
#define DEBUG "-d="
#define MAX_ITERATIONS "-i="
#define SIMULATOR "-s="
#define GLOBAL_FO_CALCULATION "-g"

#define NUMBER 0
#define STRING 1
#define YES_NO 2

#define NO 0
#define YES 1



/*************************************************************
    A FEW OPTJRPARAMETERS HELPER FUNCTION
*************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void Usage(); //used by optJrParameters constructor if the number of parameters is incorrect
char* parseArg(char * string, char * gap, int type); //used by optJrParameters constructor

#endif
