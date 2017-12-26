#ifndef STATISTICS_HH
#define STATISTICS_HH

#include <iostream>
#include <vector>
#include "optjrparameters.hh"

/**
  Statistic_iter includes relevant statistical information about a single iteration in local_search
 */
 class Statistic_iter
 {

	 int iteration;		///< Iteration number
	 int size;			///< Size of the candidates list at the considered iteration
	 double FO_Total;	///< total objective function value for that interation

 public:
	 Statistic_iter(int iter, int s, double FO): iteration(iter), size(s), FO_Total(FO){};
	 int get_iteration();
	 int get_size();
	 double get_FO_Total();

 };


class Statistics{
/**
	A vector of "Statistic_iter" object is used to store statistical information about the local search.
*/
std::vector<Statistic_iter> statistics;

public:
/**
	"add_statistics" is used to add information about an iteration to the "Statistics" object.
*/
void add_statistics(int iteration, int size, double FO_total);

/**
	"read_statistics" shows the statistics about localsearch iterations.
*/
void read_statistics(OPT_JR_parameters &par);

};

#endif
