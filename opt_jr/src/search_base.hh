#ifndef SEARCH_BASE_HH
#define SEARCH_BASE_HH

#include "batch.hh"



class Search_base
{
protected:
  /**
  Batch object as described in Batch class
  */
  Batch App_manager;

public:
  Search_base(Batch app_m): App_manager(app_m){};


  virtual void localSearch(Configuration &configuration, MYSQL *conn,  optJrParameters &par)=0;


  /**
  It calls write_results member function of App_manager private member

  */

  void write_results(MYSQL *conn, char * dbName,  optJrParameters &par);
  /**
  It prints the output of App_manager.show_solution
  */

  void print_solution();

  Batch get_app_manager();




};
#endif
