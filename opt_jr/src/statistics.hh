#ifndef STATISTICS_HH
#define STATISTICS_HH

#include <iostream>
#include <vector>
#include "optJrParameters.hh"

/**
  Statistic includes relevant statistical information about a single iteration in localSearch
 */
 class Statistic
 {

	 int iteration;		///< Iteration number
	 int size;			///< Size of the candidates list at the considered iteration
	 double FO_Total;	///< total objective function value for that interation

 public:
	 Statistic(int iter, int s, double FO): iteration(iter), size(s), FO_Total(FO){};
	 int get_iteration();
	 int get_size();
	 double get_FO_Total();

 };


class sStatistics{
/**
	A vector of "Statistic" object is used to store statistical information about the local search.
*/
std::vector<Statistic> statistics;

public:
/**
	"addStatistics" is used to add information about an iteration to the "sStatistics" object.
*/
void addStatistics(int iteration, int size, double FO_total);

/**
	"readStatistics" shows the statistics about localsearch iterations.
*/
void readStatistics(optJrParameters &par);

};

#endif
