#ifndef SEARCH_BASE_HH
#define SEARCH_BASE_HH

#include "batch.hh"


/**
Search_base is an abstract class which is used to perform dynamic overloading of
the local_search method in the children Search class.
It also provides methods to print results and save them in the database.
*/
class Search_base
{
protected:

  ///Batch object as described in Batch class
  Batch app_manager;

public:

  Search_base(Batch app_m): app_manager(app_m){};


  ///local_search is a pure virtual function: it's overloaded in search children class.
  virtual void local_search(Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par)=0;



  /**
  write_results saves results in the database
  (it calls write_results member function of app_manager).
  */
  void write_results(MYSQL *conn, char * dbName,  Opt_jr_parameters &par);

  /**
  It prints the actual solution.
  (it calls show_solution member function of app_manager).
  */
  void print_solution();



};
#endif
