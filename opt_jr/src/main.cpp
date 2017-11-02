#include <iostream>
#include <string>
#include <mysql.h>

#include "optjrparameters.hh"
#include "containers.hh"
#include "debugmessage.hh"
#include "db.hh"


int main(int argc, char **argv)
{
  /*
    LETTURA DA COMMAND LINE
    TODO: capire warnings
    TODO: capire come calcola cache
  */

  optJrParameters par(argv,argc); //memorizzo in oggetto "par" i parametri di esecuzione del programma
  debugMessage1(par);

  /*
    LEGGERE FILE DI CONFIGURAZIONE (WSI)
    -memorizzo nel vettore di coppie di char*  configuration
  */

  sConfiguration configuration = readConfigurationFile();
  debugMessage2(configuration);

  //TODO: aprire connessione con DB

  /* Connect to the db */

  MYSQL *conn = DBopen(
                const_cast<char*>( getConfigurationValue(configuration, "OptDB_IP").c_str()),
            const_cast<char*>(getConfigurationValue(configuration, "OptDB_user").c_str()),
            const_cast<char*>(getConfigurationValue(configuration, "OptDB_pass").c_str()),
            const_cast<char*>(getConfigurationValue(configuration, "OptDB_dbName").c_str())
            );


  if (conn == NULL) DBerror(conn, "open_db: Opening the database");





  //TODO: leggere file .csv  (C++)
  //TODO: chiamare calculate_NU (C++) (and NU_1)
  //TODO: chiamare calculate_bounds (C++, openMP)
  //TODO: fixInitialSolution
  //TODO: localSearch (cuore del programma, C++, MPI)
  //TODO: scrivere su database



}
