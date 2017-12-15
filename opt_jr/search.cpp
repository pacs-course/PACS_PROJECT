#include "search.hh"

#include "debugmessage.hh"
#include "utility.hh"
#include "objectiveFunction.hh"
#include "statistics.hh"


#include <string>





/*
 * 		Name:					localSearch
 * 		Description:			Localsearch algorithm as per functional analysis
 *
 */

 void Search::localSearch(sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)
 {

   std::string debugMsg;
   Policy P;
   P.localSearch(App_manager, configuration, conn, par);
 }





 void Search::writeResults(MYSQL *conn, char * dbName,  optJrParameters &par)
 {
   App_manager.writeResults(conn, dbName, par);
 }





 void Search::print_solution()
 {
   std::cout<< App_manager.show_solution();

 }
