#ifndef CANDIDATES_HH
#define CANDIDATES_HH

#include <list>
#include <vector>
#include "application.hh"


/**
   "Candidate" class is an auxiliary class used by localSearch; it stores data
   about pairs of application and the consequent changes on the objective function
   after cores exchange
*/

class Candidate{

 public:

  /* First application */

  Application  *app_i;      ///< Pointer to the first Application

	int newCoreAssignment_i;	///< Application cores after the move (first application)
	int delta_i;				      ///< Delta cores following the move (first application)
	double real_i;				    ///< Real predictor value calculated (MPI) after the interpolation (first application)

	/* Second application */

  Application  *app_j;       ///< Pointer to the second Application
	int newCoreAssignment_j;   ///< Application cores after the move (second application)
	int delta_j;               ///< Delta cores following the move (second application)
	double real_j;             ///< Real predictor value calculated (MPI) after the interpolation (second application)


	int nodes_i, nodes_j;

	double deltaFO;				///< Delta Objective Function following the move


  /// Constructor
  Candidate(Application *i, Application *j, int NCi, int NCj, double D_FO, int d_i, int d_j ): app_i(i), app_j(j), newCoreAssignment_i(NCi), newCoreAssignment_j(NCj),deltaFO(D_FO), delta_i(d_i), delta_j(d_j) {};



};




/**
list container used in localSearch to store Candidates with increasing deltaFO
*/
using sCandidates = std::list<Candidate>;

/**
  addCandidate stores build a "Candidate" object and stores it in a sCandidates container
  ordered by increasing delta FO
*/
void addCandidate(sCandidates  &cand,  Application &app_i, Application &app_j,
                  int contr1, int contr2, double delta, double delta_i, double delta_j);






#endif
