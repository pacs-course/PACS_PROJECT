

#include <iostream>
#include <string>
#include <mysql.h>
#include <vector>
#include <sys/time.h>
#include <memory>

#include "opt_jr_parameters.hh"
#include "configuration.hh"

#include "db.hh"
#include "application.hh"
#include "read_app_file.hh"
#include "batch.hh"
#include "bounds.hh"
#include "search.hh"
#include "objective_fun.hh"
#include "candidates.hh"
#include "utility.hh"
#include "search_factory.hh"


int main(int argc, char **argv)
{
  std::string debugMsg; // string to store debug messages
  std::string Msg;      //// string to store useful messages

  struct timeval  tv_initial_main,
  tv_initial_bounds,
  tv_final_bounds,
  tv_initial_nu,
  tv_final_nu,
  tv_initial_init,
  tv_final_init,
  tv_initial_fix,
  tv_final_fix,
  tv_initial_locals,
  tv_final_locals,
  tv_final_main;


  /**
  1) read informations from "wsi_config.xml" file and save it in a "Configuration" object (which is unordered_map(string,string))
  */

  Configuration configuration; //automatically initialized


  /**
  2)  Read execution parameters from command line and configuration file and save them in an "Opt_jr_parameters" object
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"****************  READING EXECUTION PARAMETERS  *******************\n";
  std::cout<<"*******************************************************************\n";


  Opt_jr_parameters par(argv,argc); // object "par" has execution parameters
  par.set_number_of_threads(configuration); // set the number of threads as specified in configuration file


  std::cout<<"<check message>: parameters for the execution of OPT_JR_CPP:\n\n";
  std::cout<<"filename: "<<par.get_filename()<<std::endl;
  std::cout<<"debug: "<<par.get_debug()<<std::endl;
  std::cout<<"cache: "<<par.get_cache()<<std::endl;
  std::cout<<"global_FO_calculation: "<<par.get_global_FO_calculation()<<std::endl;
  std::cout<<"K: "<<par.get_K()<<std::endl;
  std::cout<<"number: "<<par.get_number()<<std::endl;
  std::cout<<"max_iteration: "<<par.get_max_iterations()<<std::endl;
  std::cout<<"number_of_threads: "<<par.get_number_of_threads()<<std::endl;
  std::cout <<"Search type: "<<par.get_search_type()<<std::endl;

  std::cout<<"*******************************************************************\n\n\n";








  /**
  3) Connect to the Database
  */

  debugMsg = "\n*******************************************************************\n";
  debugMsg += "*************     CONNECTING TO THE DATABASE       ****************\n";
  debugMsg += "*******************************************************************\n\n"; par.debug_message(debugMsg);



  MYSQL *conn = DBopen(
    const_cast<char*>(configuration["OptDB_IP"].c_str()),
    const_cast<char*>(configuration["DB_port"].c_str()),
    const_cast<char*>(configuration["OptDB_user"].c_str()),
    const_cast<char*>(configuration["OptDB_pass"].c_str()),
    const_cast<char*>(configuration["OptDB_dbName"].c_str())
  );

  if (conn == NULL) DBerror(conn, (char*)"open_db: Opening the database");



  // Calculate the time taken
  gettimeofday(&tv_initial_main, NULL);


  /**
  4) Open  *.csv  file with Applications data, and save it in a "Batch" object
  */

  debugMsg = "\n*******************************************************************\n";
  debugMsg += "***********     READING .csv FILE WITH APPLICATIONS     ***********\n";
  debugMsg += "*******************************************************************"; par.debug_message(debugMsg);


  std::string folder = configuration["UPLOAD_HOME"];
  std::string filename = folder+ "/"+ par.get_filename();
  FILE* stream = fopen(filename.c_str(), "r");


  if (stream == NULL)
  {
    std::cout << "FATAL ERROR: could not find or open " << filename <<std::endl;
    exit(-1);
  }

  /* CREATE A BATCH OBJECT */
  Batch app_manager(read_app_file(stream));
  fclose(stream);


  debugMsg= app_manager.show_session_app_id();
  debugMsg +="**************************************************\n\n\n"; par.debug_message(debugMsg);


  /**
  5) Calculate bounds for each application loaded (with the calculateBounds method of Bounds class)
  */

  Msg = "\n*******************************************************************\n";
  Msg += "****************       COMPUTING BOUNDS         *******************\n";
  Msg += "*******************************************************************\n"; //par.debug_message(debugMsg);
  std::cout << Msg;


  gettimeofday(&tv_initial_bounds, NULL);

  /* CREATE A BOUNDS OBJECT */
  Bounds Bounds_eval(app_manager);

  Bounds_eval.calculate_bounds( configuration, conn, par );



  /* UPDATE BUTCH OBJECT WITH BOUNDS */
  app_manager= Bounds_eval.get_app_manager();

  gettimeofday(&tv_final_bounds, NULL);


  Msg="\n Final Bound results: ";
  Msg+=app_manager.show_bounds();
  Msg += "\n*****************    END COMPUTING BOUNDS     ********************\n\n\n"; //par.debug_message(debugMsg);
  std::cout << Msg;


  /**
  6) Calculate nu indices for each application (with the calculate_nu method of Batch class).
  It also initializes the number of cores.
  */
  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="************       COMPUTING NU INDICES         *******************\n";
  debugMsg +="*******************************************************************\n"; par.debug_message(debugMsg);

  gettimeofday(&tv_initial_nu, NULL);

  app_manager.calculate_nu(par);

  gettimeofday(&tv_final_nu, NULL);

  debugMsg ="\n*********************    END COMPUTING NU INDICES    ********************\n\n\n"; par.debug_message(debugMsg);


  /**
  7) Fix initial solution (with the fix_initial_solution method of Batch class)
  */

  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="*******************    FIXING INITIAL SOLUTION   ******************\n";
  debugMsg +="*******************************************************************\n\n"; par.debug_message(debugMsg);


  gettimeofday(&tv_initial_fix, NULL);

  app_manager.fix_initial_solution(par);

  gettimeofday(&tv_final_fix, NULL);

  debugMsg ="\n*******************    END FIXING INITIAL SOLUTION    **************\n\n\n"; par.debug_message(debugMsg);




  /**
  8) Initialize Objective Function evaluation for each application (with the initalize method of Batch class)
  */

  debugMsg = "\n*******************************************************************\n";
  debugMsg +="************    INITIALIZE BASE OBJECTIVE FUNCTION   **************\n";
  debugMsg +="*******************************************************************\n\n"; par.debug_message(debugMsg);


  gettimeofday(&tv_initial_init, NULL);

  app_manager.initialize(configuration, conn, par);

  gettimeofday(&tv_final_init, NULL);

  debugMsg +="\n************    END INITIALIZE BASE OBJECTIVE FUNCTION   *********\n\n\n"; par.debug_message(debugMsg);


  Msg = "\n\n*******************************************************************\n";
  Msg +="*********************    INITIAL SOLUTION    **********************\n";
  Msg +="*******************************************************************\n\n";std::cout <<Msg;

  std::cout<<app_manager.show_solution();

  std::cout<< "\n**************************************************\n\n\n";


  //NB: check, to be removed!
  for (auto it=app_manager.get_begin(); it!= app_manager.get_end();it++)
  std::cout <<it->get_app_id()<<" "<<it->get_session_app_id()<<"    alpha: "<<it->get_alpha() << "    beta: "<<it->get_beta()<<std::endl;



  /**
  9) Find an "optimal" solution invoking "local_search" method (of "Search" class)
  */

  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="*******************    INVOKING  LOCAL SEARCH    ******************\n";
  debugMsg +="*******************************************************************\n\n"; par.debug_message(debugMsg);


  gettimeofday(&tv_initial_locals, NULL);


  /* CREATE A SEARCH OBJECT (using the factory) */
  auto search_eval = Search_factory::search_builder(par, app_manager);


  search_eval->local_search(configuration, conn,  par );


  search_eval->write_results(conn, const_cast<char*>(configuration["DB_dbName"].c_str()), par);

  gettimeofday(&tv_final_locals, NULL);

  debugMsg ="\n**********************    END LOCAL SEARCH   ********************\n\n\n"; par.debug_message(debugMsg);


  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"******************       FINAL SOLUTION      **********************\n";
  std::cout<<"*******************************************************************\n\n";

  search_eval->print_solution();

  std::cout<<"\n*******************************************************************\n\n";


  DBclose(conn);



  gettimeofday(&tv_final_main, NULL);


  debugMsg ="\n********************    time informations   *******************"; par.debug_message(debugMsg);

  debugMsg =  "FixInitial step elapsed time: " + std::to_string(elapsed_time(tv_initial_fix, tv_final_fix));par.debug_message(debugMsg);
  debugMsg = "Findbounds  elapsed time: " + std::to_string(elapsed_time(tv_initial_bounds, tv_final_bounds));par.debug_message(debugMsg);
  debugMsg = "Initialization FO elapsed time: " + std::to_string(elapsed_time(tv_initial_init, tv_final_init));par.debug_message(debugMsg);
  debugMsg = "Evaluation nu elapsed time: " + std::to_string(elapsed_time(tv_initial_nu, tv_final_nu));par.debug_message(debugMsg);
  debugMsg = "LocalSearch step elapsed time: " + std::to_string(elapsed_time(tv_initial_locals, tv_final_locals));par.debug_message(debugMsg);
  debugMsg = "Overall elapsed time: " + std::to_string(elapsed_time(tv_initial_main, tv_final_main));par.debug_message(debugMsg);
  debugMsg ="\n***************************************************************\n\n\n"; par.debug_message(debugMsg);






}
