#ifndef SEARCH_HH
#define SEARCH_HH


#include "ConfigurationFile.hh"
#include "batch.hh"
#include "optjrParameters.hh"
#include "candidates.hh"
#include <mysql.h>
#include "search_separing.hh"
#include "search_alterning.hh"
#include "search_base.hh"

/**
//TODO: modify descript.
  "Search" class provides methods to find a solution minimizing
   the objective function. Actually only one method is supported.

*/


template <class Policy>
class search :public Search_base {

public:

  search(Batch app_m): Search_base(app_m){};


  /**
  local_search perform a local search of a solution minimizing the objective function;
  it performs cores exchanges between pairs of application and chooses the best pair. The
  search stops when no improvements are possible or the maximum number of iteration is reached.
  The function looks before at approximated values of objective function and then
  for the potential best pairs it invokes the predictor.
  */

  void local_search(Configuration &configuration, MYSQL *conn,  OPT_JR_parameters &par)
  {

    std::string debugMsg;
    Policy P;
    P.local_search(App_manager, configuration, conn, par);
  }





};


#endif
