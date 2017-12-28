#ifndef SEARCH_SELECTOR
#define SEARCH_SELECTOR


#include <memory>
#include <iostream>

#include "search_base.hh"
#include "batch.hh"
#include "opt_jr_parameters.hh"


/**
Search_factory is a factory of search objects.
search_builder method chooses a policy for search template class
according to parameters stored in the Opt_jr_parameters object
*/
class Search_factory{

 public:

  /**
  search_builder method chooses a policy for search template class
  according to parameters stored in the Opt_jr_parameters object.
  */
  static std::unique_ptr<Search_base> search_builder(Opt_jr_parameters &par, Batch &app_manager);


};


#endif
