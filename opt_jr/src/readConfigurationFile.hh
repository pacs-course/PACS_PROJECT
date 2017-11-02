#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>

/*
sConfiguration: - container che memorizza dati file di configurazione
                - è una (unordered) map
*/
using sConfiguration=std::unordered_map<std::string,std::string> ;


/*
  -funzione per inizializzare sConfiguration
  -legge il file di configurazione wsi definito nella env. var. WSI_CONFIG_FILE
*/

sConfiguration  readConfigurationFile();



#endif
