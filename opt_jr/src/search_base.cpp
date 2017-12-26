

#include "search_base.hh"
#include <iostream>



/**
write_results saves results in the database
(it calls write_results member function of app_manager).
*/
  void Search_base::write_results(MYSQL *conn, char * dbName,  Opt_jr_parameters &par)
  {
    app_manager.write_results(conn, dbName, par);
  }

  /**
  It prints the actual solution.
  (it calls show_solution member function of app_manager).
  */
  void Search_base::print_solution()
  {
    std::cout<< app_manager.show_solution();

  }
