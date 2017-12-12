

#include <iostream>
#include <string>
#include <mysql.h>
#include <vector>
#include <sys/time.h>

#include "optjrparameters.hh"
#include "readConfigurationFile.hh"
#include "debugmessage.hh"
#include "db.hh"
#include "application.hh"
#include "read_app_file.hh"
#include "batch.hh"
#include "bounds.hh"
#include "search.hh"
#include "objectiveFunction.hh"
#include "candidates.hh"
#include "utility.hh"
#include "writeResults.hh"


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
    1) read informations from "wsi_config.xml" file and save it in a "sConfiguration" object (which is unordered_map(string,string))
  */
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     READING CONFIGURATION FILE     *******************\n";
  std::cout<<"*******************************************************************\n";


  sConfiguration configuration = readConfigurationFile();
  std::cout<<"<check message>: data in the configuration file:\n\n";
  for(auto i = configuration.begin(); i !=configuration.end();++i)
  {
    std::cout<< i->first <<": "<<i->second<<std::endl;
  }
  std::cout<<"*******************************************************************\n\n\n";


  /**
     2)  Read execution parameters from command line and configuration file and save them in an "optJrParameters" object
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"****************  READING EXECUTION PARAMETERS  *******************\n";
  std::cout<<"*******************************************************************\n";


  optJrParameters par(argv,argc); // object "par" has execution parameters
  par.set_numberOfThreads(configuration); // set the number of threads as specified in configuration file


  std::cout<<"<check message>: parameters for the execution of OPT_JR_CPP:\n\n";
  std::cout<<"filename: "<<par.get_filename()<<std::endl;
  std::cout<<"debug: "<<par.get_debug()<<std::endl;
  std::cout<<"cache: "<<par.get_cache()<<std::endl;
  std::cout<<"globalFOcalculation: "<<par.get_globalFOcalculation()<<std::endl;
  std::cout<<"K: "<<par.get_K()<<std::endl;
  std::cout<<"simulator: "<<par.get_simulator()<<std::endl;
  std::cout<<"number: "<<par.get_number()<<std::endl;
  std::cout<<"maxIteration: "<<par.get_maxIteration()<<std::endl;
  std::cout<<"numberOfThreads: "<<par.get_numberOfThreads()<<std::endl;

  std::cout<<"*******************************************************************\n\n\n";



  /**
     3) Connect to the Database
  */

  debugMsg = "\n*******************************************************************\n";
  debugMsg += "*************     CONNECTING TO THE DATABASE       ****************\n";
  debugMsg += "*******************************************************************\n\n"; debugMessage(debugMsg,par);



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
  debugMsg += "*******************************************************************"; debugMessage(debugMsg,par);


  std::string folder = configuration["UPLOAD_HOME"];
  std::string filename = folder+ "/"+ par.get_filename();
  FILE* stream = fopen(filename.c_str(), "r");


  if (stream == NULL)
  {
    std::cout << "FATAL ERROR: could not find or open " << filename <<std::endl;
    exit(-1);
  }

  /* CREATE A BUTCH OBJECT */
  Batch App_manager(readAppFile(stream));

  debugMsg= "session_app_id of loaded applications:  \n";
  for (auto it= App_manager.APPs.begin(); it!=App_manager.APPs.end();++it)
    debugMsg+= "App_ID: "+ it->session_app_id +"\n";

  debugMsg +="**************************************************\n\n\n"; debugMessage(debugMsg,par);


  /**
    5) Calculate bounds for each application loaded (with the calculateBounds method of Bounds class)
  */

  Msg = "\n*******************************************************************\n";
  Msg += "****************       COMPUTING BOUNDS         *******************\n";
  Msg += "*******************************************************************\n"; //debugMessage(debugMsg,par);
  std::cout << Msg;

  gettimeofday(&tv_initial_bounds, NULL);

  Bounds::calculateBounds(App_manager, configuration, conn, par );

  gettimeofday(&tv_final_bounds, NULL);

  Msg="\n Final Bound results: ";
  for (auto it= App_manager.APPs.begin(); it!= App_manager.APPs.end();it++)
  {
    Msg+="\nBound evaluated for Session_app_id : " + it->session_app_id + " , APP_ID: " + it->app_id +
             ", Deadline = "+ std::to_string(it->Deadline_d) + ", R =" + std::to_string(it->R_d)+
             ", bound = "+ std::to_string(it->bound) ;
  }//debugMessage(debugMsg, par);
  Msg += "\n*****************    END COMPUTING BOUNDS     ********************\n\n\n"; //debugMessage(debugMsg,par);
  std::cout << Msg;

  /**
    6) Calculate nu indices for each application (with the calculate_nu method of Batch class)
  */
  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="************       COMPUTING NU INDICES         *******************\n";
  debugMsg +="*******************************************************************\n"; debugMessage(debugMsg,par);

  gettimeofday(&tv_initial_nu, NULL);
  App_manager.calculate_nu(par);
  gettimeofday(&tv_final_nu, NULL);

  debugMsg ="\n*********************    END COMPUTING NU INDICES    ********************\n\n\n"; debugMessage(debugMsg,par);


  /**
      7) Fix initial solution (with the fixInitialSolution method of Batch class)
  */

  Msg = "\n\n*******************************************************************\n";
  Msg +="*******************    FIXING INITIAL SOLUTION   ******************\n";
  Msg +="*******************************************************************\n\n"; //debugMessage(debugMsg,par);
  std::cout << Msg;

  gettimeofday(&tv_initial_fix, NULL);

  App_manager.fixInitialSolution(par);

  gettimeofday(&tv_final_fix, NULL);


  Msg= " \nFixed Initial Solution: ";//debugMessage(debugMsg, par);
	for (auto it = App_manager.APPs.begin(); it!=App_manager.APPs.end(); ++it)
  {
    Msg += "\n Application " + it->session_app_id + ",  w = " + std::to_string(it->w)
             + " ncores = " + std::to_string(it->currentCores_d); //debugMessage(debugMsg, par);
  }
  Msg +="\n*******************    END FIXING INITIAL SOLUTION    **************\n\n\n"; debugMessage(debugMsg,par);
  std::cout << Msg;



  /**
      8) Initialize Objective Function evaluation for each application (with the initalize method of Batch class)
  */

  Msg = "\n*******************************************************************\n";
  Msg +="************    INITIALIZE BASE OBJECTIVE FUNCTION   **************\n";
  Msg +="*******************************************************************\n\n"; //debugMessage(debugMsg,par);
  std::cout << Msg;

  gettimeofday(&tv_initial_init, NULL);

  App_manager.initialize(configuration, conn, par);

  gettimeofday(&tv_final_init, NULL);


  Msg = "\n Inizialization Objective Function Results:";
  for (auto it =App_manager.APPs.begin(); it!= App_manager.APPs.end(); it++)
	{
			Msg += "\nINITIALIZE BASE FO for APP "+ it->session_app_id
                + " baseFO = " + std::to_string(it->baseFO);
	}//debugMessage(debugMsg, par);

  Msg +="\n************    END INITIALIZE BASE OBJECTIVE FUNCTION   *********\n\n\n"; debugMessage(debugMsg,par);
  std::cout<< Msg;



  /**
      9) Find an "optimal" solution invoking "localSearch" method (of "Search" class)
  */

  Msg = "\n\n*******************************************************************\n";
  Msg +="*******************    INVOKING  LOCAL SEARCH    ******************\n";
  Msg +="*******************************************************************\n\n"; //debugMessage(debugMsg,par);
  std::cout <<Msg;

  gettimeofday(&tv_initial_locals, NULL);

  Search::localSearch(configuration, conn, App_manager,  par );

  gettimeofday(&tv_final_locals, NULL);

  writeResults(conn, const_cast<char*>(configuration["DB_dbName"].c_str()),App_manager, par);


  debugMsg ="\n**********************    END LOCAL SEARCH   ********************\n\n\n"; debugMessage(debugMsg,par);


  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"******************       FINAL SOLUTION      **********************\n";
  std::cout<<"*******************************************************************\n\n";



  for (auto it = App_manager.APPs.begin(); it!=App_manager.APPs.end(); ++it)
  {

    std::cout<< " Application   " << it->session_app_id << "      w = " << it->w
             << "     ncores = " << it->currentCores_d <<  "      FO = " << it->baseFO << std::endl;
  }

  std::cout<<"\n*******************************************************************\n\n";


  DBclose(conn);

  gettimeofday(&tv_final_main, NULL);


  debugMsg ="\n********************    time informations   *******************"; debugMessage(debugMsg,par);

  debugMsg =  "FixInitial step elapsed time: " + std::to_string(elapsedTime(tv_initial_fix, tv_final_fix));debugMessage(debugMsg, par);
  debugMsg = "Findbounds  elapsed time: " + std::to_string(elapsedTime(tv_initial_bounds, tv_final_bounds));debugMessage(debugMsg, par);
  debugMsg = "Initialization elapsed time: " + std::to_string(elapsedTime(tv_initial_nu, tv_final_nu));debugMessage(debugMsg, par);
  debugMsg = "LocalSearch step elapsed time: " + std::to_string(elapsedTime(tv_initial_locals, tv_final_locals));debugMessage(debugMsg, par);
  debugMsg = "Overall elapsed time: " + std::to_string(elapsedTime(tv_initial_main, tv_final_main));debugMessage(debugMsg, par);
  debugMsg ="\n***************************************************************\n\n\n"; debugMessage(debugMsg,par);



}
