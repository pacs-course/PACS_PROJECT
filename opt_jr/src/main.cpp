

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
  std::cout<<"****************  READING EXECUTION PARAMETERS  *******************\n";
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

  debugMsg = "\n*******************************************************************\n";
  debugMsg += "****************       COMPUTING BOUNDS         *******************\n";
  debugMsg += "*******************************************************************\n\n"; debugMessage(debugMsg,par);


  Bounds::calculateBounds(App_manager, configuration, conn, par );

  debugMsg = "\n***************    END COMPUTING BOUNDS     ********************\n\n\n"; debugMessage(debugMsg,par);


  /**
    6) Calculate nu indices for each application (with the calculate_nu method of Batch class)
  */
  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="************       COMPUTING NU INDICES         *******************\n";
  debugMsg +="*******************************************************************\n"; debugMessage(debugMsg,par);

  App_manager.calculate_nu(par);

  debugMsg ="\n*********************    END COMPUTING NU INDICES    ********************\n\n\n"; debugMessage(debugMsg,par);


  /**
      7) Fix initial solution (with the fixInitialSolution method of Batch class)
  */

  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="*******************    FIXING INITIAL SOLUTION   ******************\n";
  debugMsg +="*******************************************************************\n\n"; debugMessage(debugMsg,par);


  App_manager.fixInitialSolution(par);

  debugMsg ="\n*******************    END FIXING INITIAL SOLUTION    *******************\n\n\n"; debugMessage(debugMsg,par);




  /**
      8) Initialize Objective Function evaluation for each application (with the initalize method of Batch class)
  */

  debugMsg = "\n*******************************************************************\n";
  debugMsg +="************    INITIALIZE BASE OBJECTIVE FUNCTION   **************\n";
  debugMsg +="*******************************************************************\n\n"; debugMessage(debugMsg,par);

  App_manager.initialize(configuration, conn, par);

  debugMsg ="\n************    END INITIALIZE BASE OBJECTIVE FUNCTION   **************\n\n\n"; debugMessage(debugMsg,par);




  /**
      9) Find an "optimal" solution invoking "localSearch" method (of "Search" class)
  */

  debugMsg = "\n\n*******************************************************************\n";
  debugMsg +="******************       LOCAL SEARCH         *********************\n";
  debugMsg +="*******************************************************************\n\n"; debugMessage(debugMsg,par);


  Search::localSearch(configuration, conn, App_manager,  par );
  debugMsg ="\n**********************    END LOCAL SEARCH   ********************\n\n\n"; debugMessage(debugMsg,par);


  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"******************       FINAL SOLUTION      **********************\n";
  std::cout<<"*******************************************************************\n\n";



  for (auto it = App_manager.APPs.begin(); it!=App_manager.APPs.end(); ++it)
  {

    std::cout<< " Application   " << it->session_app_id << "      w = " << it->w
             << "     ncores = " << it->currentCores_d <<  "      FO = " << it->baseFO << std::endl;
  }

  std::cout<<"\n********************   END FINAL SOLUTION   **************************\n\n";



}
