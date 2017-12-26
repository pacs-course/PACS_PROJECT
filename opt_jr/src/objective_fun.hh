#ifndef OBJECTIVE_FUNCTION_HH
#define OBJECTIVE_FUNCTION_HH




#include "application.hh"
#include "opt_jr_parameters.hh"
#include "batch.hh"


/**
  This class provides methods to evaluate the objective function in different ways.
*/
class Objective_fun {

public:
  /**
  component evaluates the contribution to the calculation of the objective function of one application.
  */
  double component(Configuration &configuration, MYSQL *conn, Application  &app, Opt_jr_parameters &par);

  /**
  component_approx computes an approximation of the objective function (and update R_d).
  */
  double component_approx(Application &App, Opt_jr_parameters &par);


  /**
  global computes the value of the total objective function.
  */
  double global(Configuration &configuration, MYSQL *conn, Batch &App_manager, Opt_jr_parameters &par);


};



#endif
