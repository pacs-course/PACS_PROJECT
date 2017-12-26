#include "configuration.hh"
#include <fstream>
#include <string>
#include <string.h>




/**
The constructor inizializes the configuration container; it reads the
file defined in the environmental variable WSI_CONFIG_FILE
*/
Configuration::Configuration()
{
	FILE * fp;

	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
	    char var[256], val[256],* configurationFile;


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
	    		strcpy(var, extract_item(line, (char*)"\"", (char*)"\""));
	    		strcpy(val, extract_item(line, (char*)">", (char*)"<"));
					configuration.insert(std::make_pair(var,val));
	    	}
	    }
			fclose(fp);

}


/**
extract_item parses input from configuration file.
*/
char * Configuration::extract_item( char *const string, char *const left, const char *const right)
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
