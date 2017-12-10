#ifndef APP_BY_WEIGHT_HH
#define APP_BY_WEIGHT_HH


#include <list>
#include <memory>
#include "application.hh"


/**
  appByWeight is an auxiliary list of Application* used by fixInitialSolution; through the
  addApplicationPointer it stores Application* ordered by weight "w"
 */
using appByWeight=std::list< Application* >;


/**
 *  It saves Application* in decreasing weight "w" order
 */
void addApplicationPointer( appByWeight &LP, Application &App);




#endif
