#ifndef INVOKE_PREDICTOR_HH
#define INVOKE_PREDICTOR_HH


#include "invokePredictor_helper.hh"
#include "readConfigurationFile.hh"
#include "optjrParameters.hh"
#include "debugmessage.hh"
#include "db.hh"


#include <mysql.h>

#define WHOLE_DAGSIM 0
#define RESIDUAL_DAGSIM 1

/*
 * Name: invokePredictor
 * Description: It invokes a predictor (dagSim/Lundstrom). First it checks if an estimate of the execution time is already stored in the DB; if not, it invokes the actual predictor
 * 				and stores the result on DB cache table.
 */

char* invokePredictor(sConfiguration  &configuration, MYSQL *conn, int nNodes, int currentCores,
											char * memory, int datasize,  char *sessionId, char *appId, char *stage,
											optJrParameters &par, int flagDagsim);


#endif
