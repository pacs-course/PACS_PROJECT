#ifndef BATCH_HH
#define BATCH_HH

#include <vector>
#include "opt_jr_parameters.hh"
#include "application.hh"


/**
  Batch class manages the applications; it stores applications data in a vector and it provides
  methods useful to apply before executing the bounds evaluation and the local search.
*/
class Batch{

  /// The vector stores application data
  std::vector<Application> APPs;

public:

  /// Constructor expects a vector of application
  Batch(std::vector<Application> apps): APPs(apps){};

  /// It calculates nu indices for each application and stores it in each Application object. It also initializes the number of cores.
  void calculate_nu(Opt_jr_parameters &par);

  /// For each application, a base value for the objective function is calculated.
  void initialize(Configuration  &configuration, MYSQL *conn, Opt_jr_parameters &par);

  /// It fixes the initial solution by reallocating the residual cores to the applications that may need more resources.
  void fix_initial_solution(Opt_jr_parameters &par);

  /**
   write_results prints the results of the local_search (number of cores and VM) in a DB table.
   If a result for a (session_id, application_id) already exists, then it is replaced.
   */
  void write_results(MYSQL *conn, char * dbName,  Opt_jr_parameters &par);


  /// It returns the session_app_id of all the application stored in the Batch object.
  std::string show_session_app_id();

  /// It returns the bounds of all the application stored in the Batch object.
  std::string show_bounds();

  /// It returns the actual solution.
  std::string show_solution();


  /*
    GETTER FUNCTIONS
  */
  std::vector<Application>::iterator get_begin(){return APPs.begin();};
  std::vector<Application>::const_iterator get_cbegin(){return APPs.cbegin();};
  std::vector<Application>::iterator get_end(){return APPs.end();};
  std::vector<Application>::const_iterator get_cend(){return APPs.cend();};
  bool get_empty(){return APPs.empty();};




};


#endif
