#ifndef SEARCH_ALTERNING
#define SEARCH_ALTERNING

#include "policy_methods.hh"

/**
Policy_alterning is a policy-class: it's passed as a template argument to Search in
order to set the local_search method.
The local_search methods performs a single loop in which at each iteration an approximated value for
the objective function is evaluated for each move and then the objective
function is evaluated for most profitable moves.
*/
class Policy_alterning: public Policy_methods{

public:

  /**
  The local_search methods performs a single loop in which at each iteration an approximated value for
  the objective function is evaluated for each move and then the objective
  function is evaluated for most profitable moves.
  */
  static void local_search(Batch &app_manager, Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par);



};

#endif
