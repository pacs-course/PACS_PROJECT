#ifndef OBJECTIVE_FUNCTION_HH
#define OBJECTIVE_FUNCTION_HH

#include "application.hh"
#include "optjrParameters.hh"
#include "batch.hh"


class ObjFun {

public:
  static double ObjFunctionComponent(sConfiguration &configuration, MYSQL *conn, Application  &app, optJrParameters &par);

  static double ObjFunctionComponentApprox(Application &App, optJrParameters &par);

  static double ObjFunctionGlobal(sConfiguration &configuration, MYSQL *conn, Batch &App_manager, optJrParameters &par);


};



#endif
