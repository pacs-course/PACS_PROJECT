#ifndef DEBUGMESSAGE_HH
#define DEBUGMESSAGE_HH

#include <iostream>
#include <string>
#include <vector>
#include "application.hh"

void debugMessage1(optJrParameters &par)
{
  std::cout<<"\n\n\n*******************************************************************\n";
    std::cout<<"<debug message>: i parametri memorizzati per l'esecuzione di OPT_JR_CPP sono:\n\n";
    std::cout<<"filename: "<<par.get_filename()<<std::endl;
    std::cout<<"debug: "<<par.get_debug()<<std::endl;
    std::cout<<"cache: "<<par.get_cache()<<std::endl;
    std::cout<<"globalFOcalculation: "<<par.get_globalFOcalculation()<<std::endl;
    std::cout<<"K: "<<par.get_K()<<std::endl;
    std::cout<<"simulator: "<<par.get_simulator()<<std::endl;
    std::cout<<"number: "<<par.get_number()<<std::endl;
    std::cout<<"maxIteration: "<<par.get_maxIteration()<<std::endl;
    std::cout<<"*******************************************************************\n\n\n";

}

void debugMessage2(sConfiguration &conf)
{
  std::cout<<"\n\n\n*******************************************************************\n";
  std::cout<<"<debug message>: i dati nel file di configurazione sono:\n\n";
  for(auto i = conf.begin(); i !=conf.end();++i)
  {
    std::cout<< i->first <<": "<<i->second<<std::endl;
  }
  std::cout<<"*******************************************************************\n\n\n";

}

void debugmessage3(std::vector<Application> &loaded_app){
  std::cout<<"\n\n\n*******************************************************************\n";

  std::cout<<"<debug message>: App_ID of loaded applications"<<std::endl;
  for (auto it= loaded_app.begin(); it!=loaded_app.end();++it)
    std::cout<< "App_ID: "<<it->app_id<<std::endl;
  std::cout<<"*******************************************************************\n\n\n";

}



#endif
