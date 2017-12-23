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

  //Search select(optJrParameters par);
  static std::unique_ptr<Search_base> search_builder(optJrParameters &par, Batch &App_manager);


};


#endif
