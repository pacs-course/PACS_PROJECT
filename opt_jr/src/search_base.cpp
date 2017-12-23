

#include "search_base.hh"
#include <iostream>



  /**
  It calls writeResults member function of App_manager private member

  */

  void Search_base::writeResults(MYSQL *conn, char * dbName,  optJrParameters &par)
  {
    App_manager.writeResults(conn, dbName, par);
  }

  /**
  It prints the output of App_manager.show_solution
  */

  void Search_base::print_solution()
  {
    std::cout<< App_manager.show_solution();

  }
