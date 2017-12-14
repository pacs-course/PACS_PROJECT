#ifndef SEARCH_HH
#define SEARCH_HH

#include "readConfigurationFile.hh"
#include "batch.hh"
#include "optjrParameters.hh"
#include "candidates.hh"
#include <mysql.h>

/**
  "Search" class provides methods to find a solution minimizing
   the objective function. Actually only one method is supported.

*/

class Search{

  Batch App_manager;
  sCandidates approximatedLoop( Batch &App_manager, int &iteration, optJrParameters &par );
  void checkTotalNodes(int N, Batch &App_manager);
  //static void sequencial_localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par);
  //static void openMP_localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par);
public:
  Search(Batch app_m): App_manager(app_m){};
  /**
    localSearch perform a local search of a solution minimizing the objective function;
    it performs cores exchanges between pairs of application and chooses the best pair. The
    search stops when no improvements are possible or the maximum number of iteration is reached.
    The function looks before at approximated values of objective function and then
    for the potential best pairs it invokes the predictor.
  */
  void localSearch(sConfiguration &configuration, MYSQL *conn,  optJrParameters &par);

  void writeResults(MYSQL *conn, char * dbName,  optJrParameters &par);



  void print_solution();

  Batch get_app_manager()
  {
    return App_manager;
  }

};


#endif
