#include "debugmessage.hh"

void debugMessage(std::string &string, optJrParameters &par)
{
	if (par.get_debug() == 1)
  {
    std::cout<<"\n\n\n*******************************************************************\n";
	  std::cout<<"<debug message>:"<<string<<std::endl;
    std::cout<<"*******************************************************************\n\n\n";
  }
}
