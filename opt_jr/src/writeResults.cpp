
#include "writeResults.hh"



/*
 * 		Name:					writeResults
 * 		Description:			This function prints the results of the localSearch application (number of cores and VM) in a DB table.
 * 								If a result for a (session_id, application_id) already exists, then it is replaced.
 *
 */


void writeResults(MYSQL *conn, char * dbName, Batch &App_manager, optJrParameters &par)
{
	//char debugMsg[DEBUG_MSG];
	std::string debugMsg;

	char sqlStatement[512];

	debugMsg= "writeResults"; debugMessage(debugMsg, par);
	if (App_manager.APPs.empty())
	{
		printf("FATAL ERROR: writeResults: App_manager.APPs cannot be empty\n");
		exit(-1);
	}
	//while (pointer!=NULL)
	for(auto &elem : App_manager.APPs)
	{
		"Session ID " + elem.get_session_app_id()+ " Application Id " + elem.get_app_id()  + " cores " + std::to_string(elem.currentCores_d)+ " VMs "+ std::to_string(elem.vm); debugMessage(debugMsg, par);

		// Check if the result of the computation for that session, application has been already computed and stored previously
		sprintf(sqlStatement, "select opt_id, app_id from %s.OPT_SESSIONS_RESULTS_TABLE where opt_id='%s' and app_id='%s'",
				dbName,  par.get_filename().c_str(),
				elem.get_session_app_id().c_str());
		MYSQL_ROW row = executeSQL(conn, sqlStatement, par);

		if (row == NULL)
		{
			sprintf(sqlStatement, "insert %s.OPT_SESSIONS_RESULTS_TABLE values('%s', '%s',%d, %d)",
								dbName,
								par.get_filename().c_str(),
								elem.get_session_app_id().c_str(),
								elem.currentCores_d,
								elem.vm
						);
			if (mysql_query(conn, sqlStatement))
			{
				char error[512];
				sprintf(error, " %s", sqlStatement);
				DBerror(conn, error);
			}
		}
		else // Perform an update
		{
			sprintf(sqlStatement, "update %s.OPT_SESSIONS_RESULTS_TABLE set opt_id = '%s', app_id = '%s',num_cores = %d, num_vm = %d where opt_id='%s' and app_id='%s'",
											dbName,
											par.get_filename().c_str(),
											elem.get_session_app_id().c_str(),
											elem.currentCores_d,
											elem.vm,
											par.get_filename().c_str(),
											elem.get_session_app_id().c_str()
									);
			if (mysql_query(conn, sqlStatement))
						{

							char error[512];
							sprintf(error, " %s", sqlStatement);
							DBerror(conn, error);
						}
		}

	}

}
