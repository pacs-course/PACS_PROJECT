#ifndef CANDIDATES_HH
#define CANDIDATES_HH

#include <list>
#include "application.hh"

class Candidate{
  /* First application */
	//sApplication * app_i; 		/* Application */
  Application  app_i; //NB: PER IL MOMENTO é UNA COPIA TODO: trovare soluzione piu efficiente

	int newCoreAssignment_i;	/* Application cores after the move */
	int delta_i;				/* Delta cores following the move */
	double real_i;				/* Real predictor value calculated (MPI) after the interpolation */

	/* Second application */
	//sApplication * app_j;
  Application  app_j;
	int newCoreAssignment_j;
	int delta_j;
	double real_j;
	int nodes_i, nodes_j;

	double deltaFO;				/* Delta Objective Function following the move */

	//struct Candidates *next;

};

using sCandidates=std::list<Candidate>;


#endif
