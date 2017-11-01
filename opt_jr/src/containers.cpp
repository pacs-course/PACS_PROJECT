#include "containers.hh"
#include <fstream>
#include <string.h>


/*
-This is an helper function used by readConfigurationFile
-it's used to parse input from configuration file
-defined at the end of this file
*/
char * extractItem( char *const string, char *const left, const char *const right);



sConfiguration  readConfigurationFile()
{
	FILE * fp;

	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
	    char var[256], val[256],* configurationFile;
	 //   sConfiguration *first = NULL;
	 // sConfiguration *current = NULL;

	sConfiguration config;

	configurationFile = getenv("WSI_CONFIG_FILE");
	if (configurationFile == NULL)
	{
	    printf("Fatal error: WSI_CONFIG_FILE environment variable was not defined.\n");
	    exit(-1);
	 }
	 fp = fopen(configurationFile , "r");
	 if (fp == NULL)
	 {
	  	printf("Fatal Error: parseConfigurationFile: configuration file not found in home directory: (%s)\n", configurationFile);
	   	exit(-1);
	  }
	 while ((read = getline(&line, &len, fp)) != -1)
	  {
	  	if (line != NULL)
	    	if (strstr(line, "entry key=") != NULL)
	    	{
	    		strcpy(var, extractItem(line, "\"", "\""));
	    		strcpy(val, extractItem(line, ">", "<"));
					config.push_back(std::make_pair(var,val));
	    		//addConfiguration(&first, &current, var, val);
	    	}
	    }
	    return config;
}


// (declaration upstairs)
char * extractItem( char *const string, char *const left, const char *const right)
{
    char  *head;
	  char  *tail;
	  size_t length;
	  char  *result;
	  if ((string == NULL) || (left == NULL) || (right == NULL))

	        return NULL;

	    length = strlen(left);

	    head   = strstr(string, left);

	    if (head == NULL)

	        return NULL;

	    head += length;

	    tail  = strstr(head, right);

	    if (tail == NULL)

	        return tail;

	    length = tail - head;

	    result = (char*)malloc(1 + length);

	    if (result == NULL)

	        return NULL;

	    result[length] = '\0';
	    memcpy(result, head, length);
	    return result;
}