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


    char cmd[1024];
    char path[1024];
    char lua[1024];
    char subfolder[1024];
    char statementSearch[1024], statement[1024];
    std::string debugMsg;
    char dbName[64];
    char dir[1024];
    int TOTAL_NODES = nNodes * currentCores;
    /* Create unique extension */
    struct timeval tv;
    gettimeofday(&tv,NULL);
    double extension = (double)tv.tv_usec;
    char *output1 = (char *)malloc(64);
    if (output1 == NULL)
    {
      printf("Malloc failure: invokePredictor: output1\n");
      exit(-1);
    }

    if (nNodes == 0)
    {
      printf("Fatal error: invokePredictor; nNodes cannot be 0\n");
      exit(-1);
    }
    /* Consider always the same log folder and lua file (replacing "on the fly" the number of nodes)
     This is possible because the variance between the log folders is small*/
     strcpy(path, const_cast<char*>(configuration["RESULTS_HOME"].c_str()));
     strcpy(dir, readFolder(path));


     /* Create unique dagSim output filename */
   	double systemTime;
   	double stageTime;
   	char predictorOutputPath[256];
   	sprintf(predictorOutputPath, "/tmp/outputDagsim_%lf.txt", extension);

    /* Extract nodes, cores, dataset and memory from the subfolder name */
  	char name[512];
  	strcpy(name, readFolder(path));
  	char _nodes[8], _cores[8], _memory[8], _dataset[8], temp[8];


    strcpy(_nodes, extractItem(name, "", "_"));
  	strcpy(_cores, extractItem(name, "_", "_"));strcpy(temp, _cores);
  	strcpy(_memory, extractItem(name, strcat(temp, "_"), "_"));strcpy(temp, _memory);
  	strcpy(_dataset, extractItem(name, strcat(temp, "_"), NULL));

    /* Determine the data log folder for the predictor */
    sprintf(path, "%s/%s/%s/logs", const_cast<char*>(configuration["RESULTS_HOME"].c_str()), readFolder(path), appId);
  	strcpy(subfolder, readFolder(path));


    debugMsg= "invokePredictor: session" + std::string(sessionId) + "app" + std::string(appId) + "checking cache on" + std::to_string(TOTAL_NODES) +"cores"; par.debug_message(debugMsg);

    /* Check first if the estimate time has been already calculated */
    strcpy(dbName,const_cast<char*>(configuration["OptDB_dbName"].c_str()));
    sprintf(statementSearch, "select %s.PREDICTOR_CACHE_TABLE.val \nfrom %s.PREDICTOR_CACHE_TABLE,"
              "%s.APPLICATION_PROFILE_TABLE \nwhere %s.PREDICTOR_CACHE_TABLE.is_residual = %d and "
                  "%s.PREDICTOR_CACHE_TABLE.dataset_size = "
              "%s.APPLICATION_PROFILE_TABLE.dataset_size and\n 	"
              "%s.PREDICTOR_CACHE_TABLE.application_id=\'%s\' and %s.PREDICTOR_CACHE_TABLE.dataset_size=%d  "
              "\nand %s.PREDICTOR_CACHE_TABLE.num_cores = %d and %s.PREDICTOR_CACHE_TABLE.stage = \'%s\';\n",
              dbName,
              dbName,
              dbName,
              dbName,
              flagDagsim,
              dbName,
              dbName,
              dbName,appId, dbName,datasize, dbName,TOTAL_NODES, dbName, stage);

    MYSQL_ROW row = executeSQL(conn, statementSearch, par);

    if (row == NULL || par.get_cache() == 0)
    {
      debugMsg= "Last SQL statement returned 0 rows or cache is disabled. Invoking predictor...";par.debug_message(debugMsg);

      sprintf(cmd, "%s/%s/", path, subfolder);
      sprintf(cmd, "%s*.lua", cmd);
      strcpy(lua, ls(cmd, par));

      char pattern[64], newpath[256];
      sprintf(pattern, "Nodes = %d",(nNodes*currentCores));


      /* Create unique LUA filename */

      sprintf(newpath, "/tmp/temp_%lf.lua", extension);
      writeFile(newpath, replace(readFile(lua), pattern));

      /* Using /tmp/temp.lua instead of lua variable, so the original file is never overwritten */
      sprintf(cmd, "cd %s;./dagsim.sh %s -s > %s", const_cast<char*>(configuration["DAGSIM_HOME"].c_str()), newpath, predictorOutputPath);
      debugMsg= "Executing predictor: " +std::string(cmd); par.debug_message(debugMsg);
      _run(cmd, par);

      /* Parse the time value (either total time or residual time) */
      switch(flagDagsim)
      {
        case RESIDUAL_DAGSIM:
          systemTime = atof(extractWord(extractRowN(readFile(predictorOutputPath),3), 3));
          stageTime = atof(extractWord(extractRowN(extractRowMatchingPattern(readFile(predictorOutputPath), stage),1), 4));
          sprintf(output1, "%lf", (systemTime - stageTime));
          debugMsg= "Residual time: "+ std::string(output1); par.debug_message(debugMsg);
          break;

        case WHOLE_DAGSIM:
          strcpy(output1, extractWord(extractRowN(readFile(predictorOutputPath),1),3));
          break;
        default:
            printf("FATAL ERROR: invokePredictor: unknown case %d\n", flagDagsim);
            exit(-1);
            break;
        }
        /* Check for anomalies */
        if (double_compare(atof(output1), 0) == 0)
        {
          printf("Fatal Error: invokePredictor: dagSim output was zero (%s)\n", newpath);
          exit(-1);
        }



        /* Update the db cash table with a new value */
        /* Check again that no other MPI opt_jr instance has updated the DB */
        row = executeSQL(conn, statementSearch, par);
        if (row == NULL)
        {
          sprintf(statement,"insert %s.PREDICTOR_CACHE_TABLE values('%s', %d, %d, '%s', %d, %lf);",
            const_cast<char*>(configuration["OptDB_dbName"].c_str()),
            appId,
            datasize,
            TOTAL_NODES,
            stage,
            flagDagsim,
            atof(output1)
            );
          debugMsg= "Executing" + std::string(statement); par.debug_message(debugMsg);
          if (mysql_query(conn, statement))
          {
            char error[512];
            sprintf(error, " %s", statement);
            DBerror(conn, error);
          }

        }

      } /* If no value was stored in the cache */
      else
      {
        double out = atof(row[0]);
        debugMsg="Dagsim output retrieved from DB cash: " + std::string(appId)+" "+std::to_string(out); par.debug_message(debugMsg);
        sprintf(output1, "%lf", out);
      }


      /* Return the predictor output */
    	return output1;
    }
