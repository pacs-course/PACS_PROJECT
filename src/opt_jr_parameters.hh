

#ifndef OPTJRPARAMETERS_HH
#define OPTJRPARAMETERS_HH

#define DAGSIM 0
#define LUNDSTROM 1

#include <string>
#include "configuration.hh"

/**
    Opt_jr_parameters saves parameters received from command line; once they are saved
    they are visible with public get functions
*/

class Opt_jr_parameters
{
  std::string filename; ///< The csv file
	int debug;				///< debug option: "y" prints every message, "n" only prints fatal errors
	int cache;				///< cache option: "y" makes use of the DB predictor cache table; "n" doesn't
	int global_FO_calculation;///< global FO calculation: "y" calculates at each loop of localSearch function the global objective function value, "n" doesn't
	int K;					///< Maximum depth: the search of candidates in the auxiliary list stops if this limit is exceeded
	int number;				///< Number of total cores available for the applications (N)
	int max_iterations;		///< The maximum number of iterations in LocalSearch
	int number_of_threads;	///< The number of MPI threads
  std::string search_type; ///< type of localsearch to be used (alternating/separing)
public:
  /// The constructor takes in input all the input from command line.
  Opt_jr_parameters(char **args, int argc);

  /// set_number_of_threads sets the number_of_threads: it looks in configuration file (0== "no parallelization")
  void set_number_of_threads(Configuration &configuration);

  /// If debug is activated the message in string is printed.
  void debug_message(std::string &string);


  /*
  	Get Functions
  */
  const std::string get_filename(){ return filename;}
  const int get_debug(){	return debug;  }
  const int get_cache(){	return cache;}
  const int get_global_FO_calculation(){	return global_FO_calculation;}
  const int get_K(){	return K;}
  const int get_number(){return number;}
  const int get_max_iterations(){ return max_iterations; }
  const int get_number_of_threads(){ return number_of_threads;}
  const std::string get_search_type(){ return search_type;}


};

#endif
