#ifndef BOUNDS_HH
#define BOUNDS_HH

#include "batch.hh"
#include "readConfigurationFile.hh"
#include "optjrparameters.hh"
#include <mysql.h>
#include <string.h>


/**
  Bounds class provide a method to evaluate the bound for the applications in BATCH i.e.
  the minimal number of cores necessary to finish the execution before the deadline

*/
class Bounds {

  Batch app_manager;

  // helper function, see .cpp file for detailed description
  void  Bound(sConfiguration &configuration, MYSQL *conn, Application &app,
              optJrParameters &par, int flagDagsim);
  // helper function, see .cpp file for detailed description
  void findBound(sConfiguration &configuration, MYSQL *conn, char* db,  Application &app,
                 optJrParameters &par);
public:

  Bounds(Batch app_m): app_manager(app_m){};

  /**
  calculateBounds evaluates the bound for the applications in BATCH
  i.e. the minimal number of cores necessary to finish the execution before the deadline.
  The function looks before if the result is already stored in the database, otherwise it
  invokes the predictor doing a "HILL CLIMBING". If the number of threads in the configuration file is
  greater than 0, it does the computations in parallel (using openMP).
  */
  void calculateBounds( sConfiguration &configuration,
                       MYSQL *conn,  optJrParameters &par);

  Batch get_app_manager(){
    return app_manager;
  }

};

#endif
