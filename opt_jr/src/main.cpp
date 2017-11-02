#include <iostream>
#include <string>
#include <mysql.h>
#include <vector>

#include "optjrparameters.hh"
#include "containers.hh"
#include "debugmessage.hh"
#include "db.hh"
#include "application.hh"
#include "read_app_file.hh"

int main(int argc, char **argv)
{
  /*
    LETTURA DA COMMAND LINE (AND SAVE IT)
    TODO: capire warnings
    TODO: capire come calcola cache
  */

  optJrParameters par(argv,argc); //memorizzo in oggetto "par" i parametri di esecuzione del programma
  debugMessage1(par);

  /*
    LEGGERE FILE DI CONFIGURAZIONE (WSI) (AND SAVE IT)
    -memorizzo in configuration (sConfiguration è unordered_map(string,string) )
  */

  sConfiguration configuration = readConfigurationFile();
  debugMessage2(configuration);

  /*
   CONNECT TO THE DATABASE
   TODO: lasciare const_cast e c.str() o "pulire" main e modificare DBopen?
  */

  MYSQL *conn = DBopen(
                const_cast<char*>(configuration["OptDB_IP"].c_str()),
            const_cast<char*>(configuration["OptDB_user"].c_str()),
            const_cast<char*>(configuration["OptDB_pass"].c_str()),
            const_cast<char*>(configuration["OptDB_dbName"].c_str())
            );

  if (conn == NULL) DBerror(conn, "open_db: Opening the database");


  /*
    OPEN .csv FILE WITH APPLICATIONS, READ IT (AND SAVE IT)
  */

  std::string folder = configuration["UPLOAD_HOME"];
  std::string filename = folder+ "/"+ par.get_filename();
  FILE* stream = fopen(filename.c_str(), "r");


  if (stream == NULL)
  {
    printf("FATAL ERROR: could not find or open %s\n", filename);
    exit(-1);
  }

  std::vector<Application> loaded_app=readAppFile(stream);
  debugmessage3(loaded_app);



  //TODO: chiamare calculate_NU (C++) (and NU_1)


  //TODO: chiamare calculate_bounds (C++, openMP)


  //TODO: fixInitialSolution

  //TODO: localSearch (cuore del programma, C++, MPI)

  //TODO: scrivere su database

}
