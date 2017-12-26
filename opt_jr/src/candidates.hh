#ifndef CANDIDATES_HH
#define CANDIDATES_HH

#include <list>
#include <vector>
#include "application.hh"


/**
   Candidate_pair class is an auxiliary class included in Candidates which is used by localSearch; it stores data
   about pairs of application and the consequent changes on the objective function
   after cores exchange.
*/

class Candidate_pair{

  /* Static values */

 Application  app_i;      ///< Copy of the first Application
 Application  app_j;       ///< Copy of the second Application

 int new_core_assignment_i;	///< Cores after the move (first application)
 int new_core_assignment_j;   ///< Cores after the move (second application)

 double delta_fo;				///< Delta Objective Function following the move
 int delta_i;				      ///< Delta cores following the move (first application)
 int delta_j;               ///< Delta cores following the move (second application)


 /* Dynamic values */

 double real_i;				    ///< Real predictor value calculated (MPI) after the interpolation (first application)
 double real_j;             ///< Real predictor value calculated (MPI) after the interpolation (second application)



 public:

  /// Constructor
  Candidate_pair(Application i, Application j, int NCi, int NCj, double D_FO, int d_i, int d_j ): app_i(i), app_j(j), new_core_assignment_i(NCi), new_core_assignment_j(NCj),delta_fo(D_FO), delta_i(d_i), delta_j(d_j) {};

  const std::string get_app_id_i (){return app_i.get_app_id();}
  const std::string get_app_id_j (){return app_j.get_app_id();}
  const std::string get_session_app_id_i (){return app_i.get_session_app_id();}
  const std::string get_session_app_id_j (){return app_j.get_session_app_id();}
  const double get_delta_fo(){return delta_fo;}
  const int get_new_core_assignment_i(){return new_core_assignment_i ;}
  const int get_new_core_assignment_j(){return new_core_assignment_j;}

  double get_real_i(){return real_i;}
  double get_real_j(){return real_j;}
  const double get_base_fo_i(){return app_i.get_baseFO();}
  const double get_base_fo_j(){return app_j.get_baseFO();}
  const int get_currentCores_d_i(){return app_i.get_currentCores_d();}
  const int get_currentCores_d_j(){return app_j.get_currentCores_d();}
  Application get_app_i(){return app_i;}
  Application get_app_j(){return app_j;}
  Application& get_app_i_ref(){return app_i;}
  Application& get_app_j_ref(){return app_j;}

  void set_real_i(double d){real_i=d;}
  void set_real_j(double d){real_j=d;}



};




/**
Auxiliary class used by localSearch; it stores pairs of applications for which a cores exchange
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
  void invoke_predictor_openMP( OPT_JR_parameters &par, Configuration  &configuration );

  /**
  "invoke_predictor_seq" calls sequencially the component for each pair of application
  and it stores the results for each pair in real_i, real_j.
  */
  void invoke_predictor_seq( MYSQL *conn, OPT_JR_parameters &par, Configuration  &configuration );


  int get_size(){return cand.size(); }
  bool get_empty(){return cand.empty();}
  std::list<Candidate_pair>::iterator get_begin(){return cand.begin();}
  std::list<Candidate_pair>::const_iterator get_cbegin(){return cand.cbegin();}
  std::list<Candidate_pair>::iterator get_end(){return cand.end();}
  std::list<Candidate_pair>::const_iterator get_cend(){return cand.cend();}


};


#endif
