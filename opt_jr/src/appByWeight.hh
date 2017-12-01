#ifndef APP_BY_WEIGHT_HH
#define APP_BY_WEIGHT_HH


#include <list>
#include <memory>
#include "application.hh"


/*
 *  appByWeight è un container di app* che ordina le app per peso
 */


using appByWeight=std::list< Application* >;


void addApplicationPointer( appByWeight &LP, Application &App);




#endif
