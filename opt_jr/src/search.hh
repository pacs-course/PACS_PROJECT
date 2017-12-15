#ifndef SEARCH_HH
#define SEARCH_HH


#include "readConfigurationFile.hh"
#include "batch.hh"
#include "optjrParameters.hh"
#include "candidates.hh"
#include <mysql.h>
#include "search_separing.hh"
#include "search_alterning.hh"

/**
//TODO: modify descript.
  "Search" class provides methods to find a solution minimizing
   the objective function. Actually only one method is supported.

*/


template <class Policy>
class search :public Policy {


  /**
  Batch object as described in Batch class
  */
  Batch App_manager;






  /**
  It estimates the objective function for each move.
  The candidate applications for which the move is profitable are stored in a
  sCandidate object
  */
  //sCandidates approximatedLoop( Batch &App_manager, int &iteration, optJrParameters &par );

  /**
  It checks that the total allocated nodes is still less or equal
  than the total number of cores available N
  */
  //void checkTotalNodes(int N, Batch &App_manager);

public:

  search(Batch app_m): App_manager(app_m){};

  /**
  localSearch perform a local search of a solution minimizing the objective function;
  it performs cores exchanges between pairs of application and chooses the best pair. The
  search stops when no improvements are possible or the maximum number of iteration is reached.
  The function looks before at approximated values of objective function and then
  for the potential best pairs it invokes the predictor.
  */

  void localSearch(sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)
  {

    std::string debugMsg;
    Policy P;
    P.localSearch(App_manager, configuration, conn, par);
  }

  /**
  It calls writeResults member function of App_manager private member

  */

  void writeResults(MYSQL *conn, char * dbName,  optJrParameters &par)
  {
    App_manager.writeResults(conn, dbName, par);
  }

  /**
  It prints the output of App_manager.show_solution
  */

  void print_solution()
  {
    std::cout<< App_manager.show_solution();

  }


  Batch get_app_manager(){    return App_manager;}

};


#endif
