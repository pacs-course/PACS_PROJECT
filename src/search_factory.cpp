#include "search_factory.hh"

#include "policy_alterning.hh"
#include "policy_separing.hh"
#include "search.hh"


/**
search_builder method chooses a policy for search template class
according to parameters stored in the Opt_jr_parameters object.
*/
std::unique_ptr<Search_base> Search_factory::search_builder(Opt_jr_parameters &par, Batch &app_manager)
{
  if (par.get_search_type()=="a"|| par.get_search_type()=="A")
  return std::unique_ptr<Search_base>(new Search<Policy_alterning>(app_manager));
  else if (par.get_search_type()=="s" || par.get_search_type()=="S")
  return std::unique_ptr<Search_base>(new Search<Policy_separing>(app_manager));
  else
  {
    std::cout<< "FATAL ERROR: unknown search type "<<std::endl;
    exit(-1);
  }


}
