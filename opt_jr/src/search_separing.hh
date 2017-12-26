#ifndef SEARCH_SEPARING
#define SEARCH_SEPARING

#include "search_methods.hh"
#include "batch.hh"

# define N_APPROX_LOOP 10

/**
Search_separing is a policy-class: it's passed as a template argument to Search in
order to set the local_search method.
The local_search methods performs first an approximated loop in which the most profitable
move is done at each iteration, then it performs an exact loop in which the objective
function is evaluated for all possible pairs exchanges at each iteration.
*/
class Search_separing : public Search_methods
{

public:
  /**
  The local_search methods performs first an approximated loop in which the most profitable
  move is done at each iteration, then it performs an exact loop in which the objective
  function is evaluated for all possible pairs exchanges at each iteration.
  */
  void local_search(Batch &app_manager, Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par);

};

#endif
