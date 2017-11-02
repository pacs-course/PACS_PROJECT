#ifndef READ_APP_FILE_HH
#define READ_APP_FILE_HH

#include <fstream>
#include <stdio.h>
#include <vector>
#include "application.hh"


const int MAX_LINE_LENGTH = 1024;
const int _SESSION_APP_ID = 1;
const int _APP_ID = 2;
const int _W = 3;
const int _CHI_0 = 4;
const int _CHI_C = 5;
const int _M = 6;
const int _m = 7;
const int _V = 8;
const int _v = 9;
const int _D = 10;
const int _St = 11;
const int _Dsz = 12;


/* Number of values in the csv file */
const int PARAMETERS = 12;


/*
 * 		Name:					getfield
 * 		Input parameters:		char * source, int num
 * 		Output parameters:		A word
 * 		Description:			it extracts values from the csv file
 *
 */

char * getfield(char* line, int num);
std::vector<Application> readAppFile(FILE* stream);

#endif