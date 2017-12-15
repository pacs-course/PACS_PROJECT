

#ifndef OPTJRPARAMETERS_HH
#define OPTJRPARAMETERS_HH

#define DAGSIM 0
#define LUNDSTROM 1

#include <string>
#include "readConfigurationFile.hh"

/**
    optJrParameters saves parameters received from command line; once they are saved
    they are visible with public get_*() functions
*/

class optJrParameters
{
  std::string filename; ///< The csv file
	int debug;				///< debug option: "y" prints every message, "n" only prints fatal errors
	int cache;				///< cache option: "y" makes use of the DB predictor cache table; "n" doesn't
	int globalFOcalculation;///< global FO calculation: "y" calculates at each loop of localSearch function the global objective function value, "n" doesn't
	int K;					///< Maximum depth: the search of candidates in the auxiliary list stops if this limit is exceeded
	int simulator;			///< The simulator type: either dagSim or Lundstrom
	int number;				///< Number of total cores available for the applications (N)
	int maxIterations;		///< The maximum number of iterations in LocalSearch
	int numberOfThreads;	///< The number of MPI threads
  std::string search_type="a"; //TODO : to be changed and passed from command line 
public:
  /// The constructor takes in input all the input from command line
  optJrParameters(char **args, int argc);
  /// Set the number of threads: it looks in configuration file (0== "no parallelization")
  void set_numberOfThreads(sConfiguration &configuration);

  /*
  	Get Functions
  */
  const std::string get_filename(){ return filename;}
  const int get_debug(){	return debug;  }
  const int get_cache(){	return cache;}
  const int get_globalFOcalculation(){	return globalFOcalculation;}
  const int get_K(){	return K;}
  const int get_simulator(){return simulator;}
  const int get_number(){return number;}
  const int get_maxIteration(){ return maxIterations; }
  const int get_numberOfThreads(){ return numberOfThreads;}
  const std::string get_search_type(){ return search_type;}


};

#endif
