#include <iostream>
#include <string>

#include "optjrparameters.hh"
#include "containers.hh"
#include "debugmessage.hh"

int main(int argc, char **argv)
{
  /*
    LEGGERE DA COMMAND LINE
    TODO: capire warnings
    TODO: capire come calcola cache
  */

  optJrParameters par(argv,argc); //memorizzo in "par" i parametri di esecuzione del programma
  debugMessage1(par);

  /*
    LEGGERE FILE DI CONFIGURAZIONE (WSI)
  */

  sConfiguration configuration = readConfigurationFile();
  debugMessage2(configuration);







  //TODO: aprire connessione con DB
  //TODO: leggere file .csv  (C++)
  //TODO: chiamare calculate_NU (C++) (and NU_1)
  //TODO: chiamare calculate_bounds (C++, openMP)
  //TODO: fixInitialSolution
  //TODO: localSearch (cuore del programma, C++, MPI)
  //TODO: scrivere su database



}
