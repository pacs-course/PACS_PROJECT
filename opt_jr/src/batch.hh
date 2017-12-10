#ifndef BATCH_HH
#define BATCH_HH

#include <vector>
#include "optJrParameters.hh"
#include "application.hh"


/**
  This class manage the applications; it stores application data in a vector and provide
  methods useful to apply before executing the localSearch
*/
class Batch{

public:
  /// The vector stores application data
  std::vector<Application> APPs;

  /// Constructor expects a vector of application which should be given by the "readAppFile" function
  Batch(std::vector<Application> apps): APPs(apps){};

  /// It calculates nu indices for each application and stores it in the "Application" object
  void calculate_nu(optJrParameters &par);

  /// For each application, a base value for the objective function is calculated.
  void initialize(sConfiguration  &configuration, MYSQL *conn, optJrParameters &par);

  /// It fixes the initial solution by reallocating the residual cores to the applications that may need more resources
  void fixInitialSolution(optJrParameters &par);

};


#endif
