#ifndef HELPER_HH
#define HELPER_HH

/*************************************************************
    A FEW OPTJRPARAMETERS HELPER FUNCTION
*************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void Usage(); //used by optJrParameters constructor if the number of parameters is incorrect
char* parseArg(char * string, char * gap, int type); //used by optJrParameters constructor

#endif
