#include <iostream>
#include <string>
#include "optjrparameters.hh"

int main(int argc, char **argv)
{
  std::cout<< "<debug message>: makefile works"<<std::endl;

  /*
   leggere da command line
   TODO: capire warnings
   TODO: capire come calcola cache
   */

  optJrParameters par(argv,argc); //memorizzo in "par" i parametri di esecuzione del programma

  std::cout<<"<debug message>: i parametri memorizzati per l'esecuzione di OPT_JR_CPP sono:"<<std::endl;
  std::cout<<"filename: "<<par.get_filename()<<std::endl;
  std::cout<<"debug: "<<par.get_debug()<<std::endl;
  std::cout<<"cache: "<<par.get_cache()<<std::endl;
  std::cout<<"globalFOcalculation: "<<par.get_globalFOcalculation()<<std::endl;
  std::cout<<"K: "<<par.get_K()<<std::endl;
  std::cout<<"simulator: "<<par.get_simulator()<<std::endl;
  std::cout<<"number: "<<par.get_number()<<std::endl;
  std::cout<<"maxIteration: "<<par.get_maxIteration()<<std::endl;


  //TODO: leggere file config (C)
  




  //TODO: aprire connessione con DB
  //TODO: leggere file .csv  (C++)
  //TODO: chiamare calculate_NU (C++) (and NU_1)
  //TODO: chiamare calculate_bounds (C++, openMP)
  //TODO: fixInitialSolution
  //TODO: localSearch (cuore del programma, C++, MPI)
  //TODO: scrivere su database



}
