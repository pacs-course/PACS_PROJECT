#ifndef BATCH_HH
#define BATCH_HH

#include <vector>
#include "optJrParameters.hh"
#include "application.hh"
#include "candidates.hh"

class Batch{

public:
  std::vector<Application> APPs;
  Batch(std::vector<Application> apps): APPs(apps){};
  void calculate_nu(optJrParameters &par);
  void initialize(sConfiguration  &configuration, MYSQL *conn, optJrParameters &par);
  //void fixInitialSolution(sApplication &applications,   optJrParameters &par)
  sCandidates approximatedLoop( int &iteration, optJrParameters &par );

};


#endif
