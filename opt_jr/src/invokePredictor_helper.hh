#ifndef INVOKEPREDICTOR_HELPER_HH
#define INVOKEPREDICTOR_HELPER_HH

#include "optJrParameters.hh"
#include "utility.hh"

#include <string.h>
#include <stdio.h>




//Those are helper functions used by invokePredictor

char * readFolder(char *  path);
void writeFile(const char *filepath, const char *data);
char * ls(char * pattern, optJrParameters &par);
char * extractRowN(char *text, int row);
char * replace(char * text, char *newLine);
char * readFile(char * filename);
char * _run(char * cmd, optJrParameters &par);
char * extractWord(char * line, int pos);
char * extractRowMatchingPattern(char *text, char *pattern);



#endif
