#ifndef SEARCH_SEPARING
#define SEARCH_SEPARING

#include "search_methods.hh"
#include "batch.hh"

# define N_APPROX_LOOP 10

class Search_separing : public Search_methods
{

public:
  void local_search(Batch &app_manager, Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par);

};

#endif
