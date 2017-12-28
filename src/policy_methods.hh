#ifndef SEARCH_METHODS_HH
#define SEARCH_METHODS_HH

#include "configuration.hh"
#include "batch.hh"
#include "opt_jr_parameters.hh"
#include "candidates.hh"
#include  <mysql.h>


/**
  "Policy_methods" class provides methods to find a solution minimizing
   the objective function. Those methods are used by children classes.
*/
class Policy_methods{

public:

  ///It checks that the total allocated nodes is still less or equal than the total number of cores available N
  static void check_total_nodes(int N, Batch &app_manager);

  /**
  It estimates the objective function for each move. The pairs of applications for which the move
  is profitable are stored in a Candidates object (which is returned).
  */
  static Candidates approximated_loop( Batch &app_manager, Opt_jr_parameters &par);

  /**
  It evaluates the delta FO for all the candidates in app_pairs and it does the best change (if it exist) in app_manager
  */
  static void exact_loop(Candidates &app_pairs, Configuration &configuration,  MYSQL *conn,  Batch &app_manager, Opt_jr_parameters &par, int &index_pair);



};

#endif
