#ifndef CANDIDATES_HH
#define CANDIDATES_HH

#include <list>


#include "candidate_pair.hh"
#include "opt_jr_parameters.hh"



/**
Auxiliary class used by local_search; it stores pairs of applications for which a cores exchange
could be profitable and provide methods to add Candidate_pair and evaluate in parallel the objective function

*/

class Candidates
{

  /**
  Container where applications candidates to change cores are stored in order of increasing deltaFO
  */
  std::list<Candidate_pair> cand;

public:

  /**
  "add_candidate" build a "Candidate_pair" object and stores it in a Candidates container
  ordered by increasing delta FO
  */
  void add_candidate(Application app_i, Application app_j,int contr1, int contr2, double delta, double delta_i, double delta_j);


  /**
  "invoke_predictor_openMP" calls in parallel the component for each pair of application
  and it stores the results for each pair in real_i, real_j.
  */
  void invoke_predictor_openMP( Opt_jr_parameters &par, Configuration  &configuration );

  /**
  "invoke_predictor_seq" calls sequencially the component for each pair of application
  and it stores the results for each pair in real_i, real_j.
  */
  void invoke_predictor_seq( MYSQL *conn, Opt_jr_parameters &par, Configuration  &configuration );


  int get_size(){return cand.size(); }
  bool get_empty(){return cand.empty();}
  std::list<Candidate_pair>::iterator get_begin(){return cand.begin();}
  std::list<Candidate_pair>::const_iterator get_cbegin(){return cand.cbegin();}
  std::list<Candidate_pair>::iterator get_end(){return cand.end();}
  std::list<Candidate_pair>::const_iterator get_cend(){return cand.cend();}


};


#endif
