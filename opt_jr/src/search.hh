#ifndef SEARCH_HH
#define SEARCH_HH


#include "configuration.hh"
#include "batch.hh"
#include "opt_jr_parameters.hh"
#include "candidates.hh"
#include <mysql.h>
#include "search_separing.hh"
#include "search_alterning.hh"
#include "search_base.hh"


/**
Search template class provides methods to find a solution minimizing
the objective function.
It's derived from Search_base class in order to allow dynamic polymorphism.
It expects as template argument a Policy class which implements a local_search method.
*/
template <class Policy>
class Search :public Search_base {

public:

  /// Search class uses the base class constructor.
  Search(Batch app_m): Search_base(app_m){};


  /**
  local_search calls the local_search method defined in the Policy class
  */
  void local_search(Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par)
  {
    std::string debugMsg;
    Policy P;
    P.local_search(app_manager, configuration, conn, par);
  }





};


#endif
