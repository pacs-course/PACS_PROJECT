#ifndef BOUNDS_HH
#define BOUNDS_HH

#include "batch.hh"
#include "readConfigurationFile.hh"
#include "optjrparameters.hh"
#include <mysql.h>
#include <string.h>


class Bounds {

  static void  Bound(sConfiguration &configuration, MYSQL *conn, Application &app,
              optJrParameters &par, int flagDagsim);
  static void findBound(sConfiguration &configuration, MYSQL *conn, char* db,  Application &app,
                 optJrParameters &par);
public:
  static void calculateBounds(Batch  & app_manager, int n_threads, sConfiguration &configuration,
                       MYSQL *conn,  optJrParameters &par);

};

#endif
