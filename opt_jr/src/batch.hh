#ifndef BATCH_HH
#define BATCH_HH

#include <vector>
#include "optJrParameters.hh"
#include "application.hh"

class Batch{
std::vector<Application> APPs;

public:
Batch(std::vector<Application> apps): APPs(apps){};

void calculate_nu(optJrParameters &par);

};


#endif
