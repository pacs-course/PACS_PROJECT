#ifndef APP_BY_WEIGHT_HH
#define APP_BY_WEIGHT_HH

#include <queue>
#include "application.hh"

/*
 *  appByWeight è un container di app che ordina le app per peso
 */

//usata in appByWeight
class Compare2
{
public:
    bool operator() (const Application& a, const Application& b);
       //NB: siccome voglio ordine crescente e di default priority queue è decrescente, uso il maggiore !
};


/*coppie di app ordinate per weight crescente!*/
using appByWeight=std::priority_queue<Application,std::vector<Application>,Compare2>;
//NB: è giusto usare vector come underlying container?


#endif
