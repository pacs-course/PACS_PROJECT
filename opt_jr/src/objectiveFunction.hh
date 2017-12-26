#ifndef OBJECTIVE_FUNCTION_HH
#define OBJECTIVE_FUNCTION_HH




#include "application.hh"
#include "optjrParameters.hh"
#include "batch.hh"


/**
  This class provides methods to evaluate the objective function in different ways
*/

class Objective_fun {

public:
  /**
   component evaluates the contribution to the calculation of the objective function of one application.
   Currently, only one method is supported. Note that the algorithm's choice is stored in the "mode" field
   of the application structure.
   */
  double component(Configuration &configuration, MYSQL *conn, Application  &app, OPT_JR_parameters &par);

  /**
  component_approx computes an approximation of the objective function (and update R_d)
  */
  double component_approx(Application &App, OPT_JR_parameters &par);


  /**
  global computes the value of the total objective function
  */
  double global(Configuration &configuration, MYSQL *conn, Batch &App_manager, OPT_JR_parameters &par);


};



#endif
