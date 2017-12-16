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


  virtual void localSearch(sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)=0;


  /**
  It calls writeResults member function of App_manager private member

  */

  void writeResults(MYSQL *conn, char * dbName,  optJrParameters &par)
  {
    App_manager.writeResults(conn, dbName, par);
  }

  /**
  It prints the output of App_manager.show_solution
  */

  void print_solution()
  {
    std::cout<< App_manager.show_solution();

  }

  Batch get_app_manager(){    return App_manager;}




};
#endif
