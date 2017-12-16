#include "search_factory.hh"

#include "search_alterning.hh"
#include "search_separing.hh"
#include "search.hh"


std::unique_ptr<Search_base> Search_factory::search_builder(optJrParameters &par, Batch &App_manager)
{
  if (par.get_search_type()=="a")
  return std::unique_ptr<Search_base>(new search<Search_alterning>(App_manager));
  else if (par.get_search_type()=="s")
  return std::unique_ptr<Search_base>(new search<Search_separing>(App_manager));
  else
  {
    std::cout<< "FATAL ERROR: unknown search type "<<std::endl;
    exit(-1);
  }


}
