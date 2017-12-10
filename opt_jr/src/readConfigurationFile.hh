#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>

/**
  sConfiguration is a container which memorize data from the configuration file; it's an unordered map
*/
using sConfiguration=std::unordered_map<std::string,std::string> ;


/**
  This function inizializes the sConfiguration container; it reads the
  file defined in the environmental variable WSI_CONFIG_FILE
*/

sConfiguration  readConfigurationFile();

/**
This is an helper function used by readConfigurationFile;
it's used to parse input from configuration file.
(not changed from original C version)
*/
char * extractItem( char *const string, char *const left, const char *const right);



#endif
