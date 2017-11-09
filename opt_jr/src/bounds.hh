#ifndef BOUNDS_HH
#define BOUNDS_HH

#include "batch.hh"
#include "readConfigurationFile.hh"
#include "optjrparameters.hh"
#include <mysql.h>


class Bounds{

public:
  void calculateBounds(Batch  & app_manager, int n_threads, sConfiguration &configuration,
                       MYSQL *conn,  optJrParameters &par);

};

#endif
