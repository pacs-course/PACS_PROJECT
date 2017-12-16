#ifndef SEARCH_SEPARING
#define SEARCH_SEPARING

#include "search_methods.hh"
#include "batch.hh"

class Search_separing : public Search_methods
{

public:
  void localSearch(Batch &app_manager, sConfiguration &configuration, MYSQL *conn,  optJrParameters &par);

};

#endif
