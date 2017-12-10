#ifndef OBJECTIVE_FUNCTION_HH
#define OBJECTIVE_FUNCTION_HH

#include "application.hh"
#include "optjrParameters.hh"
#include "batch.hh"

/**
  This class provides methods to evaluate the objective function in different ways
*/

class ObjFun {

public:
  /**
   It evaluates the contribution to the calculation of the objective function of one application.
   Currently, only one method is supported. Note that the algorithm's choice is stored in the "mode" field
   of the application structure.
   */
  static double ObjFunctionComponent(sConfiguration &configuration, MYSQL *conn, Application  &app, optJrParameters &par);

  /**
  It computes an approximation of the objective function (and update R_d)
  */
  static double ObjFunctionComponentApprox(Application &App, optJrParameters &par);


  /**
  It calculates the value of the total objective function
  */
  static double ObjFunctionGlobal(sConfiguration &configuration, MYSQL *conn, Batch &App_manager, optJrParameters &par);


};



#endif
