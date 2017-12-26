#ifndef SEARCH_METHODS_HH
#define SEARCH_METHODS_HH

#include "configuration.hh"
#include "batch.hh"
#include "opt_jr_parameters.hh"
#include "candidates.hh"
#include  <mysql.h>

/**
  "Search_methods" class provides methods to find a solution minimizing
   the objective function.

*/

class Search_methods{

public:
  void check_total_nodes(int N, Batch &App_manager);
  /**
   It estimates the objective function for each move. The candidate applications for which the move is profitable are stored in a sCandidate object
  */

  Candidates approximated_loop( Batch &App_manager,/* int &iteration,*/ Opt_jr_parameters &par);

  /**
  It evaluates the delta FO for all the candidates in app_pairs and it does the best change (if it exist) in App_manager
  */
  void exact_loop(Candidates &app_pairs, Configuration &configuration,  MYSQL *conn,  Batch &App_manager, Opt_jr_parameters &par, int &index_pair);



};

#endif
