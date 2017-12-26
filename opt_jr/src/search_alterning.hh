#ifndef SEARCH_ALTERNING
#define SEARCH_ALTERNING

#include "search_methods.hh"

class Search_alterning: public Search_methods{

public:
  void local_search(Batch &app_manager, Configuration &configuration, MYSQL *conn,  OPT_JR_parameters &par);



};

#endif
