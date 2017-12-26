

#include "search_base.hh"
#include <iostream>



  /**
  It calls write_results member function of app_manager private member

  */

  void Search_base::write_results(MYSQL *conn, char * dbName,  Opt_jr_parameters &par)
  {
    app_manager.write_results(conn, dbName, par);
  }

  /**
  It prints the output of app_manager.show_solution
  */

  void Search_base::print_solution()
  {
    std::cout<< app_manager.show_solution();

  }
