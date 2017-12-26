#ifndef SEARCH_SELECTOR
#define SEARCH_SELECTOR

//#include "search.hh"
#include <memory>
#include <iostream>

#include "search_base.hh"
#include "batch.hh"

#include "optjrParameters.hh"


class Search_factory{
public:

  //Search select(OPT_JR_parameters par);
  static std::unique_ptr<Search_base> search_builder(OPT_JR_parameters &par, Batch &App_manager);


};


#endif
