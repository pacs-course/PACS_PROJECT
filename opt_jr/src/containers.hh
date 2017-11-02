#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <vector>
#include <utility>
#include <string>

/*
sConfiguration: - container che memorizza dati file di configurazione
                 - è un vettore di coppie di char*
*/
using sConfiguration=std::vector<std::pair<std::string,std::string>> ;
// maybe better to replace it with a unordered_map key-value?
//using sConfiguration=std::vector<std::pair<char*,char*>> ;

// funzione per inizializzare sConfiguration:
sConfiguration  readConfigurationFile();

// Restitisce valore variabile di configurazione desiserata
std::string getConfigurationValue(sConfiguration &conf, std::string s);
//char* getConfigurationValue(sConfiguration &conf, char* s);

//NB: con un' unordered_map posso accedervi per chiave-> non serve getConfigurationValue! 





#endif
