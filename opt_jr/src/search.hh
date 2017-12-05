#ifndef SEARCH_HH
#define SEARCH_HH

#include "readConfigurationFile.hh"
#include "batch.hh"
#include "optjrParameters.hh"
#include <mysql.h>


class Search{

  static void checkTotalNodes(int N, Batch &App_manager);

public:
  static void localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par);

};


#endif
