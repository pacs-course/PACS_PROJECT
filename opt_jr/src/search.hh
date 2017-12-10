#ifndef SEARCH_HH
#define SEARCH_HH

#include "readConfigurationFile.hh"
#include "batch.hh"
#include "optjrParameters.hh"
#include "candidates.hh"
#include <mysql.h>

/**
  "Search" class provides methods to find a solution minimizing
   the objective function. Actually only one method is supported

*/

class Search{

  static sCandidates approximatedLoop( Batch &App_manager, int &iteration, optJrParameters &par );
  static void checkTotalNodes(int N, Batch &App_manager);

public:
  /**
    "localSearch" perform a local search of a solution minimizing the objective function;
    it performs cores exchanges between pairs of application and chooses the best pair. The
    search stops when no improvements are possible or the maximum number of iteration is reached.
    The function before look at approximated values of objective function and then
    for the potential best pairs it invokes the predictor.
  */
  static void localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par);

};


#endif
