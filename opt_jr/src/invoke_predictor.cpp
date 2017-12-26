#include "invoke_predictor.hh"

#include <iostream>



#include <string>
#include <string.h>



 /**
 invoke_predictor invokes the dagSim predictor.
 First it checks if an estimate of the execution time is already stored in the DB;
 if not, it invokes the predictor and stores the result on DB cache table.
 */
 char* invoke_predictor(Configuration  &configuration, MYSQL *conn, int nNodes, int currentCores,
	 char * memory, int datasize,  char *sessionId, char *appId, char *stage,
	 Opt_jr_parameters &par, int flagDagsim)
	 {


		 char parameters[1024];
		 char cmd[1024];
		 char path[1024];
		 char lua[1024];
		 char subfolder[1024];
		 char *output1 = (char *)malloc(64);

		 char statement[1024];
		 char dbName[64];
		 char dir[1024];



		 if (output1 == NULL)
		 {
			 printf("Malloc failure: invoke_predictor: output1\n");
			 exit(-1);
		 }

		 std::string debugMsg=" Invoking predictor "; par.debug_message(debugMsg);



		 // Consider always the same folder and lua file (replacing the number of nodes)
		 // This is possible because the variance between the log folders is small

		 strcpy(path, const_cast<char*>(configuration["RESULTS_HOME"].c_str()));
		 strcpy(dir, readFolder(path));

		 switch(par.get_simulator())
		 {
			 case LUNDSTROM:
			 sprintf(parameters, "%d %d %s %d %s", nNodes, currentCores, memory, datasize, appId);
			 sprintf(cmd, "cd %s;python run.py %s", const_cast<char*>(configuration["LUNDSTROM_HOME"].c_str()), parameters);
			 break;
			 case DAGSIM:

			 MYSQL_ROW row=NULL;
			 if (par.get_cache()==1)
			 {
				 debugMsg= "Chosen simulator: DAGSIM"; par.debug_message(debugMsg);

				 // Check if there is an output from dagSim already cached in the DB


				 strcpy(dbName, const_cast<char*>(configuration["OptDB_dbName"].c_str()));
				 sprintf(statement, "select %s.PREDICTOR_CACHE_TABLE.val \nfrom %s.PREDICTOR_CACHE_TABLE,"
				 "%s.APPLICATION_PROFILE_TABLE \nwhere %s.PREDICTOR_CACHE_TABLE.is_residual = %d and "
				 "%s.PREDICTOR_CACHE_TABLE.dataset_size = "
				 "%s.APPLICATION_PROFILE_TABLE.dataset_size and\n 	"
				 "%s.PREDICTOR_CACHE_TABLE.application_id=\'%s\' and %s.PREDICTOR_CACHE_TABLE.dataset_size=%d  "
				 "and %s.PREDICTOR_CACHE_TABLE.num_cores = %d;\n",
				 dbName,
				 dbName,
				 dbName,
				 dbName,
				 flagDagsim,
				 dbName,
				 dbName,
				 dbName,appId, dbName,datasize, dbName,currentCores);


				 debugMsg= "From invoke_predictor executing SQL STATEMENT below "; par.debug_message(debugMsg);
				 row = executeSQL(conn, statement, par);
			 }



			 if (row == NULL || par.get_cache()==0)
			 {
				 if (par.get_cache()==1)
				 debugMsg= "Last SQL statement returned 0 rows. Invoking predictor..."; par.debug_message(debugMsg);

				 // Replaced FAKE variable with RESULTS_HOME
				 sprintf(path, "%s/%s/%s/logs", const_cast<char*>(configuration["RESULTS_HOME"].c_str()), readFolder(path), appId);


				 strcpy(subfolder, readFolder(path));
				 sprintf(cmd, "%s/%s/", path, subfolder);
				 sprintf(cmd, "%s*.lua", cmd);

				 strcpy(lua, ls(cmd, par));

				 char pattern[64];

				 sprintf(pattern, "Nodes = %d",(nNodes*currentCores));

				 writeFile("/tmp/temp.lua", replace(readFile(lua), pattern));

				 double systemTime;
				 double stageTime;

				 // Using /tmp/temp.lua instead of lua variable, so the original file is never overwritten

				 sprintf(cmd, "cd %s;./dagsim.sh %s -s > /tmp/outputDagsim.txt", const_cast<char*>(configuration["DAGSIM_HOME"].c_str()), "/tmp/temp.lua");
				 debugMsg="Executing predictor: "; debugMsg+= cmd; par.debug_message(debugMsg);

				 _run(cmd, par);
				 std::cout << "FLAGDAGSIM = "<<flagDagsim<<std::endl;

				 switch(flagDagsim)
				 {
					 case RESIDUAL_DAGSIM:
					 systemTime = atof(extractWord(extractRowN(readFile((char*)"/tmp/outputDagsim.txt"),3), 3));
					 stageTime = atof(extractWord(extractRowN(extractRowMatchingPattern(readFile((char*)"/tmp/outputDagsim.txt"), stage),1), 4));

					 sprintf(output1, "%lf", (systemTime - stageTime));
					 debugMsg="Residual time: "; debugMsg+=output1; par.debug_message(debugMsg);
					 break;
					 case WHOLE_DAGSIM:
					 strcpy(output1, extractWord(extractRowN(readFile((char*)"/tmp/outputDagsim.txt"),1),3));
					 break;
					 default:
					 printf("FATAL ERROR: invoke_predictor: unknown case %d\n", flagDagsim);
					 exit(-1);
					 break;
				 }


				 if (double_compare(atof(output1), 0) == 0)
				 {
					 printf("Fatal Error: invoke_predictor: dagSim output was zero (%s)\n", lua);
					 exit(-1);
				 }


				 if (par.get_cache()==1) //if cache is on
				 {
					 // Update the db cash table with a new value
					 char statement[1024];

					 sprintf(statement,"insert %s.PREDICTOR_CACHE_TABLE values('%s', %d, %d, '%s', %d, %lf);",
					 const_cast<char*>(configuration["OptDB_dbName"].c_str()),
					 appId,
					 datasize,
					 nNodes*currentCores,
					 stage,
					 flagDagsim,
					 atof(output1)
				 );


				 if (mysql_query(conn, statement))
				 {
					 char error[512];
					 sprintf(error, " %s", statement);
					 DBerror(conn, error);
				 }
			 }
		 }
		 else
		 {
			 double out = atof(row[0]);
			 debugMsg="Dagsim output retrieved from DB cash: "; debugMsg+=appId; debugMsg+=" ";debugMsg+= std::to_string(out); par.debug_message(debugMsg);
			 sprintf(output1, "%lf", out);
		 }
	 }


	 return output1;
 }
