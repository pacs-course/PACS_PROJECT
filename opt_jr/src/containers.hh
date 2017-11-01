#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <vector>
#include <utility>
#include <string>

/*
sConfiguration: - container che memorizza dati file di configurazione
                 - è un vettore di coppie di stringhe
*/
using sConfiguration=std::vector<std::pair<std::string,std::string>> ;
sConfiguration  readConfigurationFile(); // funzione per inizializzare sConfiguration







#endif
