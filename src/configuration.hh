#ifndef CONTAINERS_HH
#define CONTAINERS_HH


#include <utility>
#include <string>
#include <unordered_map>


/**
  Configuration is a class which stores data from the configuration file.
*/
class Configuration{

  /**
  extract_item parses input from configuration file.
  */
  char * extract_item( char *const string, char *const left, const char *const right);

  /**
  configuration is a container which memorize data from the configuration file; it's an unordered map
  */
  std::unordered_map<std::string,std::string> configuration;

public:

  /**
  The constructor inizializes the configuration container; it reads the
  file defined in the environmental variable WSI_CONFIG_FILE
  */
  Configuration();

  /**
  Overloading of operator [] allows easy access to configuration data.
  */
  std::string operator[](std::string s){return configuration[s];}


};

#endif
