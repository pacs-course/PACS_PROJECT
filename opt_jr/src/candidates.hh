#ifndef CANDIDATES_HH
#define CANDIDATES_HH

#include <list>
#include <vector>
#include "application.hh"


class Candidate{

 public:

  /* First application */

  Application  *app_i;

	int newCoreAssignment_i;	/* Application cores after the move */
	int delta_i;				/* Delta cores following the move */
	double real_i;				/* Real predictor value calculated (MPI) after the interpolation */

	/* Second application */

  Application  *app_j;
	int newCoreAssignment_j;
	int delta_j;
	double real_j;
	int nodes_i, nodes_j;

	double deltaFO;				/* Delta Objective Function following the move */

  Candidate(Application *i, Application *j, int NCi, int NCj, double D_FO, int d_i, int d_j ): app_i(i), app_j(j), newCoreAssignment_i(NCi), newCoreAssignment_j(NCj),deltaFO(D_FO), delta_i(d_i), delta_j(d_j) {};



};


//usata in sCandidates
/*
class Compare1
{
public:
    bool operator() (const Candidate& a, const Candidate& b);
       //NB: siccome voglio ordine crescente e di default priority queue è decrescente, uso il maggiore !
};
*/


/*coppie di app ordinate per deltaFO crescente!*/
using sCandidates = std::list<Candidate>;


void addCandidate(sCandidates  &cand,  Application &app_i, Application &app_j,
                  int contr1, int contr2, double delta, double delta_i, double delta_j);






#endif
