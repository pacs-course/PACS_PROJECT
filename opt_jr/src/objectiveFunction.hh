#ifndef OBJECTIVE_FUNCTION_HH
#define OBJECTIVE_FUNCTION_HH

#include "application.hh"
#include "optjrParameters.hh"


class ObjFun {

public:
  static double ObjFunctionComponentApprox(Application &App, optJrParameters &par);

};



#endif
