#ifndef CANDIDATE_PAIR_HH
#define CANDIDATE_PAIR_HH

#include "application.hh"


/**
   Candidate_pair class is an auxiliary class included in Candidates class which is used by local_search; it stores data
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

  /// Constructor expects all "non editable" values.
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


#endif
