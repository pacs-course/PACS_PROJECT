#ifndef OPTJRPARAMETERS_HH
#define OPTJRPARAMETERS_HH

#include <string>
#include "common.hh"
#include "helper.hh"

/*
optJrParameters: classe che memorizza parametri inviati da command line al programma
*/

class optJrParameters
{
  std::string filename;
  int debug;
  int cache;
  int globalFOcalculation;
  int K;
  int simulator;
  int number;
  int maxIterations;
public:
  optJrParameters(char **args, int argc);
  const std::string get_filename();
  const int get_debug();
  const int get_cache();
  const int get_globalFOcalculation();
  const int get_K();
  const int get_simulator();
  const int get_number();
  const int get_maxIteration();
};

#endif
