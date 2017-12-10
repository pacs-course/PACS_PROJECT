

#ifndef OPTJRPARAMETERS_HH
#define OPTJRPARAMETERS_HH

#define DAGSIM 0
#define LUNDSTROM 1

#include <string>

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
	//int numberOfThreads;	///< The number of MPI threads
public:
  /// The constructor takes in input all the input from command line
  optJrParameters(char **args, int argc);
  /// Returns the name of the file with applications
  const std::string get_filename();
  /// Returns the debug option (1==YES, 0==NO)
  const int get_debug();
  /// Returns the cache option (1==YES, 0==NO)
  const int get_cache();
  /// Returns the option globalFOcalculation (1==YES, 0==NO)
  const int get_globalFOcalculation();
  /// Returns K
  const int get_K();
  /// Returns which simulator is used
  const int get_simulator();
  /// Returns the available number of cores
  const int get_number();
  /// Returns the maximum number of iteration for localSearch
  const int get_maxIteration();
};

#endif
