

#include <iostream>
#include <string>
#include <mysql.h>
#include <vector>

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



int main(int argc, char **argv)
{
  std::string debugMsg; // string to store debug messages



  /**
    1) read informations from "wsi_config.xml" file and save it in a "sConfiguration" object (which is unordered_map(string,string))
  */
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     READING CONFIGURATION FILE     *******************\n";
  std::cout<<"*******************************************************************\n";


  sConfiguration configuration = readConfigurationFile();
  std::cout<<"<check message>: i dati nel file di configurazione sono:\n\n";
  for(auto i = configuration.begin(); i !=configuration.end();++i)
  {
    std::cout<< i->first <<": "<<i->second<<std::endl;
  }
  std::cout<<"*******************************************************************\n\n\n";


  /**
     2)  Read execution parameters from command line and configuration file and save them in an "optJrParameters" object
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"**************** READING EXECUTION PARAMETERS ******************\n";
  std::cout<<"*******************************************************************\n";


  optJrParameters par(argv,argc); // object "par" has execution parameters
  par.set_numberOfThreads(configuration); // set the number of threads as specified in configuration file


  std::cout<<"<check message>: i parametri memorizzati per l'esecuzione di OPT_JR_CPP sono:\n\n";
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

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     CONNECTING TO THE DATABASE       *****************\n";
  std::cout<<"*******************************************************************\n";

  MYSQL *conn = DBopen(
                const_cast<char*>(configuration["OptDB_IP"].c_str()),
            const_cast<char*>(configuration["OptDB_user"].c_str()),
            const_cast<char*>(configuration["OptDB_pass"].c_str()),
            const_cast<char*>(configuration["OptDB_dbName"].c_str())
            );

  if (conn == NULL) DBerror(conn, (char*)"open_db: Opening the database");

  std::cout<<"*******************************************************************\n\n\n";



  /**
    4) Open  *.csv  file with Applications data, and save it in a "Batch" object
  */
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     READ .CSV FILE WITH APPS       *******************\n";
  std::cout<<"*******************************************************************\n";

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

  std::cout<<"<check message>: session_app_id of loaded applications"<<std::endl;
  for (auto it= App_manager.APPs.begin(); it!=App_manager.APPs.end();++it)
    std::cout<< "App_ID: "<<it->session_app_id<<std::endl;
  std::cout<<"*******************************************************************\n\n\n";


  /**
    5) Calculate bounds for each application loaded (with the calculateBounds method of Bounds class)
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"****************       CALCULATE BOUNDS         *******************\n";
  std::cout<<"*******************************************************************\n\n";

  int n_threads=1;
  Bounds::calculateBounds(App_manager,n_threads, configuration, conn, par );

  std::cout<<"*******************************************************************\n\n\n";


  /**
    6) Calculate nu indices for each application (with the calculate_nu method of Batch class)
  */
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************       CALCULATE NU INDICES         *******************\n";
  std::cout<<"*******************************************************************\n";

  App_manager.calculate_nu(par);

  std::cout<<"*******************************************************************\n\n\n";


  /**
      7) Fix initial solution (with the fixInitialSolution method of Batch class)
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"********************    FIX INITIAL SOLUTION   ********************\n";
  std::cout<<"*******************************************************************\n\n";


  App_manager.fixInitialSolution(par);


  std::cout<<"*******************************************************************\n\n\n";



  /**
      8) Initialize Objective Function evaluation for each application (with the initalize method of Batch class)
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************    INITIALIZE BASE OBJECTIVE FUNCTION   **************\n";
  std::cout<<"*******************************************************************\n\n";

  App_manager.initialize(configuration, conn, par);


  debugMsg= "end calculate base objective function";
  std::cout<<"*******************************************************************\n\n\n";




  /**
      9) Find an "optimal" solution invoking "localSearch" method (of "Search" class)
  */

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"******************       LOCAL SEARCH         *********************\n";
  std::cout<<"*******************************************************************\n\n";


  Search::localSearch(configuration, conn, App_manager,  par );
  std::cout<<"*******************************************************************\n\n\n";


  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"******************       FINAL SOLUTION      *********************\n";
  std::cout<<"*******************************************************************\n\n";



  for (auto it = App_manager.APPs.begin(); it!=App_manager.APPs.end(); ++it)
  {

    std::cout<< " Application   " << it->session_app_id << "      w = " << it->w
             << "     ncores = " << it->currentCores_d <<  "      FO = " << it->baseFO << std::endl;
  }

  std::cout<<"*******************************************************************\n\n";










}
