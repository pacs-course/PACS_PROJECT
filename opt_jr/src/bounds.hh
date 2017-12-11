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

  // helper function, see .cpp file for detailed description
  static void  Bound(sConfiguration &configuration, MYSQL *conn, Application &app,
              optJrParameters &par, int flagDagsim);
  // helper function, see .cpp file for detailed description
  static void findBound(sConfiguration &configuration, MYSQL *conn, char* db,  Application &app,
                 optJrParameters &par);
public:

  /**
  evaluate in parallel (using openMP) the bound for the applications in BATCH
  i.e. the minimal number of cores necessary to finish the execution before the deadline.
  The function looks before if the result is already stored in the database, otherwise it
  invokes the predictor doing a "HILL CLIMBING".
  */
  static void calculateBounds(Batch  & app_manager, sConfiguration &configuration,
                       MYSQL *conn,  optJrParameters &par);

};

#endif
