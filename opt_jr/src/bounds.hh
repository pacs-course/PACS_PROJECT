#ifndef BOUNDS_HH
#define BOUNDS_HH

#include "batch.hh"
#include "configuration.hh"
#include "opt_jr_parameters.hh"
#include <mysql.h>
#include <string.h>


/**
  Bounds class provide a method to evaluate the bound for the applications in BATCH i.e.
  the minimal number of cores necessary to finish the execution before the deadline

*/
class Bounds {

  /**
  Batch object as described in Batch class
  */
  Batch app_manager;

  /**
  This function calculates the bound given a certain deadline and number of cores. Predictor method is invoked until an upper bound,
  consisting of the number of nodes, is found (once that the time calculated by the predictor, a rollback is performed to
  return the last "safe" number of core and time.

  */
  void  bound(Configuration &configuration, MYSQL *conn, Application &app,
              Opt_jr_parameters &par, int flagDagsim);
  /**
  Initially, this function queries the lookup table to find the number of cores, calculated by OPT_IC earlier,
  given a deadline, an application id and a dataset size.
  Secondly, it invokes the bound function.
  */
  void find_bound(Configuration &configuration, MYSQL *conn, char* db,  Application &app,
                 Opt_jr_parameters &par);
public:

  Bounds(Batch app_m): app_manager(app_m){};

  /**
  calculate_bounds evaluates the bound for the applications in BATCH
  i.e. the minimal number of cores necessary to finish the execution before the deadline.
  The function looks before if the result is already stored in the database, otherwise it
  invokes the predictor doing a "HILL CLIMBING". If the number of threads in the configuration file is
  greater than 0, it does the computations in parallel (using openMP).
  */
  void calculate_bounds( Configuration &configuration,
                       MYSQL *conn,  Opt_jr_parameters &par);

  Batch get_app_manager(){  return app_manager;}

};

#endif
