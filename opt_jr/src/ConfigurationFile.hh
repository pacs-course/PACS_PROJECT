#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>


class Configuration{

  /**
  This is an helper function used by ConfigurationFile;
  it's used to parse input from configuration file.
  */
  char * extract_item( char *const string, char *const left, const char *const right);

  /**
  Configuration is a container which memorize data from the configuration file; it's an unordered map
  */
  //using Configuration=std::unordered_map<std::string,std::string> ;
  std::unordered_map<std::string,std::string> configuration;

public:
  /**
  The constructor inizializes the configuration container; it reads the
  file defined in the environmental variable WSI_CONFIG_FILE
  */
  Configuration();
  std::string operator[](std::string s){return configuration[s];}


};

#endif
