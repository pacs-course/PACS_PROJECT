#ifndef WRITE_RESULTS_HH
#define WRITE_RESULTS_HH

#include <stdio.h>
#include "db.hh"
#include "batch.hh"
#include "optjrParameters.hh"

/*
 * 		Name:					writeResults
 * 		Description:			This function prints the results of the localSearch application (number of cores and VM) in a DB table.
 * 								If a result for a (session_id, application_id) already exists, then it is replaced.
 *
 */


void writeResults(MYSQL *conn, char * dbName, Batch &App_manager, optJrParameters &par);



#endif
