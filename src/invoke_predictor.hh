#ifndef INVOKE_PREDICTOR_HH
#define INVOKE_PREDICTOR_HH


#include "invoke_predictor_helper.hh"
#include "configuration.hh"
#include "opt_jr_parameters.hh"

#include "db.hh"



#include <mysql.h>

#define WHOLE_DAGSIM 0
#define RESIDUAL_DAGSIM 1

/**
invoke_predictor invokes the dagSim predictor.
First it checks if an estimate of the execution time is already stored in the DB;
if not, it invokes the predictor and stores the result on DB cache table.
*/

char* invoke_predictor(Configuration  &configuration, MYSQL *conn, int nNodes, int currentCores,
	char * memory, int datasize,  char *sessionId, char *appId, char *stage,
	Opt_jr_parameters &par, int flagDagsim);







	#endif
