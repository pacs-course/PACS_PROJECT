#ifndef SEARCH_SELECTOR
#define SEARCH_SELECTOR

//#include "search.hh"
#include <memory>
#include <iostream>

#include "search_base.hh"
#include "batch.hh"

#include "opt_jr_parameters.hh"


class Search_factory{
public:

  //Search select(Opt_jr_parameters par);
  static std::unique_ptr<Search_base> search_builder(Opt_jr_parameters &par, Batch &App_manager);


};


#endif
