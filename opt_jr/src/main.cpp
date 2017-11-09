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

int main(int argc, char **argv)
{
  std::string debugMsg;
  /*********************************************************
  READ FROM COMMAND LINE (AND SAVE IT)
  *********************************************************/

  //TODO: capire warnings
  //TODO: capire come calcola cache?

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"**************** READING COMMAND-LINE PARAMETERS ******************\n";
  std::cout<<"*******************************************************************\n";


  optJrParameters par(argv,argc); //memorizzo in oggetto "par" i parametri di esecuzione del programma


  std::cout<<"<check message>: i parametri memorizzati per l'esecuzione di OPT_JR_CPP sono:\n\n";
  std::cout<<"filename: "<<par.get_filename()<<std::endl;
  std::cout<<"debug: "<<par.get_debug()<<std::endl;
  std::cout<<"cache: "<<par.get_cache()<<std::endl;
  std::cout<<"globalFOcalculation: "<<par.get_globalFOcalculation()<<std::endl;
  std::cout<<"K: "<<par.get_K()<<std::endl;
  std::cout<<"simulator: "<<par.get_simulator()<<std::endl;
  std::cout<<"number: "<<par.get_number()<<std::endl;
  std::cout<<"maxIteration: "<<par.get_maxIteration()<<std::endl;
  std::cout<<"*******************************************************************\n\n\n";


  /*********************************************************
    READ FROM CONFIGURATION FILE (WSI) (AND SAVE IT)
      -memorizzo in configuration (sConfiguration è unordered_map(string,string),
        definita con "using" in readConfigurationFile)
  *********************************************************/
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

  /*********************************************************
   CONNECT TO THE DATABASE
  *********************************************************/
  // NB: executeSQL è ancora commentata
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     CONNECTING TO THE DATABASE       *****************\n";
  std::cout<<"*******************************************************************\n";

  MYSQL *conn = DBopen(
                const_cast<char*>(configuration["OptDB_IP"].c_str()),
            const_cast<char*>(configuration["OptDB_user"].c_str()),
            const_cast<char*>(configuration["OptDB_pass"].c_str()),
            const_cast<char*>(configuration["OptDB_dbName"].c_str())
            );

  if (conn == NULL) DBerror(conn, "open_db: Opening the database");

  std::cout<<"*******************************************************************\n\n\n";



  /********************************************************
    OPEN .CSV FILE WITH APPLICATIONS, READ IT (AND SAVE IT)
  *********************************************************/
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************     READ .CSV FILE WITH APPS       *******************\n";
  std::cout<<"*******************************************************************\n";

  std::string folder = configuration["UPLOAD_HOME"];
  std::string filename = folder+ "/"+ par.get_filename();
  FILE* stream = fopen(filename.c_str(), "r");


  if (stream == NULL)
  {
    printf("FATAL ERROR: could not find or open %s\n", filename);
    exit(-1);
  }

  /* CREATE A BUTCH OBJECT */

  Batch App_manager(readAppFile(stream));//NB: si può rifare evitando creazione temporanea? (universal ref)

  std::cout<<"<check message>: App_ID of loaded applications"<<std::endl;
  for (auto it= App_manager.APPs.begin(); it!=App_manager.APPs.end();++it)
    std::cout<< "App_ID: "<<it->app_id<<std::endl;
  std::cout<<"*******************************************************************\n\n\n";


  /********************************************************
    CALCULATE NU INDICES
  *********************************************************/
  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"************       CALCULATE NU INDICES         *******************\n";
  std::cout<<"*******************************************************************\n";

  App_manager.calculate_nu(par);

  std::cout<<"*******************************************************************\n\n\n";


  /********************************************************
    CALCULATE BOUNDS
  *********************************************************/

  std::cout<<"\n\n*******************************************************************\n";
  std::cout<<"****************       CALCULATE BOUNDS         *******************\n";
  std::cout<<"*******************************************************************\n";

  Bounds myBound;
  myBound.calculateBounds(App_manager,2, configuration, conn, par );

  std::cout<<"*******************************************************************\n\n\n";



  //TODO: chiamare calculate_NU (C++) (and NU_1)


  //TODO: chiamare calculate_bounds (C++, openMP)


  //TODO: fixInitialSolution

  //TODO: localSearch (cuore del programma, C++, MPI)

  //TODO: scrivere su database

}
