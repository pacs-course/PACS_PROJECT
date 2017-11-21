#include "bounds.hh"

#include "debugmessage.hh"
#include "db.hh"
#include "invokePredictor.hh"
#include <omp.h>
#include <string>
#include <math.h>





/*
 * 		Name:				Bound
 * 		Input parameters:	int deadline, int nNodes, int nCores, int datasetSize, char *appId,
 * 		Output parameters:	int *R (initial Predictor estimate for the given cores, the bound (number of cores), and the time for the determined bound.
 * 		Description:		This function calculates the bound given a certain deadline and number of nodes, cores. Predictor method is invoked until an upper bound,
 * 							consisting of the number of nodes, is found (once that the time calculated by the predictor, a rollback is performed to
 * 							return the last "safe" number of core and time.
 *
 */

void  Bounds::Bound(sConfiguration &configuration, MYSQL *conn, Application &app, optJrParameters &par, int flagDagsim)
{
	double predictorOutput;
	//char debugMsg[DEBUG_MSG];
  std::string debugMsg;
	double BTime = 0;
	int BCores = 0;
	int STEP = app.V;
	int nCores;
	int nNodes = 1; // Temporary fix

	app.currentCores_d = app.nCores_DB_d;


	int X0 = ((int) ( app.currentCores_d / app.V) ) * app.V;
	app.currentCores_d = std::max( X0, ((int) app.V));


	nCores = app.currentCores_d;

	//TODO: implementare un metodo di app che chiama invoke predictor;
	predictorOutput = atoi(invokePredictor(configuration, conn, nNodes, nCores, "*", app.datasetSize, const_cast<char*>((app.session_app_id).c_str()),
													const_cast<char*>((app.app_id).c_str()), const_cast<char*>((app.stage).c_str()), par, flagDagsim));

	//debugMsg= "Predictor Output: "+ std::to_string(predictorOutput); debugMessage(debugMsg,par);

	debugMsg="Bound evaluation for" + app.session_app_id +
					 " predictorOutput = " + std::to_string(predictorOutput) +
					 "(deadline is " +std::to_string(app.Deadline_d)+ ") cores "
					 + std::to_string(nCores); debugMessage(debugMsg, par);
		app.sAB.index = 0;
		app.sAB.vec[app.sAB.index].nCores = nCores;
		app.sAB.vec[app.sAB.index].R = predictorOutput;
		app.sAB.index++;


		BTime = predictorOutput;


		if (doubleCompare(predictorOutput, app.Deadline_d) == 1)
			while (predictorOutput > app.Deadline_d)
			{
				if (nCores ==0)
				{
					printf("Warning Bound (| if case): nCores is currently 0 for app. Cannot invoke Predictor\n");
					nCores= app.V;
					//leave the while loop
					break;
				}

				nCores = nCores + STEP;
				predictorOutput = atof(invokePredictor(configuration, conn, nNodes, nCores, "8G", app.datasetSize, const_cast<char*>((app.session_app_id).c_str()),
																const_cast<char*>((app.app_id).c_str()), const_cast<char*>((app.stage).c_str()), par, WHOLE_DAGSIM));

				debugMsg="Bound evaluation for" + app.session_app_id +
								 " predictorOutput = " + std::to_string(predictorOutput) +
								 "(deadline is " +std::to_string(app.Deadline_d)+ ") cores "
								 + std::to_string(nCores); debugMessage(debugMsg, par);

				BCores = nCores;
				BTime = predictorOutput;


				app.sAB.vec[app.sAB.index].nCores = nCores;
				app.sAB.vec[app.sAB.index].R = predictorOutput;
				app.sAB.index = (app.sAB.index +1) % HYP_INTERPOLATION_POINTS;





				app.boundIterations++;

			}

		else
			while (doubleCompare(predictorOutput, app.Deadline_d) == -1)
			{
				BCores = nCores;
				BTime = predictorOutput;
				nCores = nCores - STEP;



				if (nCores <0)
				{
					printf("nCores is currently 0. Cannot invoke Predictor\n");
					exit(-1);
				}

				if (nCores ==0)
				{
					//printf("Warning Bound (< if case): nCores is currently 0 for app. Cannot invoke Predictor\n");
					nCores= app.V;
					//leave the while loop
					break;
				}

				predictorOutput = atof(invokePredictor(configuration, conn, nNodes, nCores, "8G", app.datasetSize, const_cast<char*>((app.session_app_id).c_str()),
				const_cast<char*>((app.app_id).c_str()), const_cast<char*>((app.stage).c_str()), par, WHOLE_DAGSIM));

				debugMsg="Bound evaluation for" + app.session_app_id +
								 " predictorOutput = " + std::to_string(predictorOutput) +
								 "(deadline is " +std::to_string(app.Deadline_d)+ ") cores "
								 + std::to_string(nCores); debugMessage(debugMsg, par);

				app.sAB.vec[app.sAB.index].nCores = nCores;
				app.sAB.vec[app.sAB.index].R = predictorOutput;
				app.sAB.index = app.sAB.index % HYP_INTERPOLATION_POINTS;

				//printf("(down) time = %d Rnew =%d\n", time, BTime);
				app.boundIterations++;
			}
	//}
	/* Update the record with bound values */

	app.currentCores_d = BCores;
	app.R_d = BTime;
	app.bound = BCores;
	debugMsg="\n\nSession_app_id : " + app.session_app_id + " , APP_ID: " + app.app_id +
					 ", D = "+ std::to_string(app.Deadline_d) + ", R =" + std::to_string(app.R_d)+
					 ", bound = "+ std::to_string(app.bound) + "\n\n"; debugMessage(debugMsg, par);


}



/*
 * 		Name:					findBound
 * 		Description:			Initially, this function queries the lookup table to find the number of cores, calculated by OPT_IC earlier,
 * 								given a deadline, an application id and a dataset size.
 * 								Secondly, it invokes the Bound function.
 *
 */

void Bounds::findBound(sConfiguration &configuration, MYSQL *conn, char* db,  Application &app, optJrParameters &par)
{
  std::string debugMsg;
  char statement[256];

  //debugMsg= "findBound "+ app.session_app_id + " " + app.app_id; debugMessage(debugMsg, par);

  ///Retrieve nCores from the DB

    sprintf(statement,
                        "select num_cores_opt, num_vm_opt from %s.OPTIMIZER_CONFIGURATION_TABLE where application_id='%s' and dataset_size=%d and deadline=%lf;"
                        , db, const_cast<char*>(app.app_id.c_str()), app.datasetSize, app.Deadline_d);
		debugMsg= "From findbound executing statement below for app "+app.app_id; debugMessage(debugMsg, par);
    MYSQL_ROW row = executeSQL(conn, statement, par);

    if (row == NULL)
    {
      printf("Fatal error: no matches found on OPTIMIZER_CONFIGURATION_TABLE.\nCHeck that OPT_IC was run over the specific application.\n");
      exit(-1);
    }

    app.nCores_DB_d = atoi(row[0]);
    app.vm = atoi(row[1]);
		debugMsg=" From findbound a first estimate on ncores:"+  std::to_string(app.nCores_DB_d)
							+" and VM: "+ std::to_string(app.vm) + " for application "+app.app_id ;debugMessage(debugMsg, par);



    Bound(configuration, conn, app, par, WHOLE_DAGSIM);
    debugMsg="A bound for " + app.session_app_id + "  (app_id: " + app.app_id + ") has been calculated";debugMessage(debugMsg, par);


}




/*
 * CALL IN PARALLEL FINDBOUND (OPENMP)
 */
void Bounds::calculateBounds(Batch  & app_manager, int n_threads,
                             sConfiguration &configuration, MYSQL *conn,
                             optJrParameters &par)
  {
    std::string debugMsg;
    debugMsg=" Calculate bounds for each application in parallel with "+ std::to_string(n_threads)+" threads (using openMP) \n" ;debugMessage(debugMsg,par);

    MYSQL *conn2[n_threads];
    for (int i =0; i< n_threads;++i)
    {
      conn2[i]=DBopen(
        const_cast<char*>(configuration["OptDB_IP"].c_str()),
        const_cast<char*>(configuration["OptDB_user"].c_str()),
        const_cast<char*>(configuration["OptDB_pass"].c_str()),
        const_cast<char*>(configuration["OptDB_dbName"].c_str())
      );
    }

    /*call findbound in parallel;*/
    #pragma omp parallel num_threads(n_threads)
    {
      int ID=omp_get_thread_num();
      int j=0;
      /* assign each app to a thread */
      for (auto it=app_manager.APPs.begin(); it !=app_manager.APPs.end(); it++)
      {
        int pos=j%n_threads;
      //  debugMsg= "pos= " + std::to_string(pos)
        //         +", j= " + std::to_string(j)
          //       + ", ID= " + std::to_string(ID);debugMessage(debugMsg,par);

        if(pos==ID)
        {
          debugMsg= "\n\n\n\n\n\n\n\n\nCall findBound of app " + app_manager.APPs[j].app_id
                    + " from thread " + std::to_string(ID); debugMessage(debugMsg,par);

          findBound(configuration, conn2[ID], const_cast<char*>(configuration["OptDB_dbName"].c_str()), app_manager.APPs[j], par);
        }
        ++j;
      }
    }

		debugMsg="\n\n************* FINAL BOUNDS RESULTS: **************";
		for (auto it= app_manager.APPs.begin(); it!= app_manager.APPs.end();it++)
		{
			debugMsg+="\nBound evaluated for " + it->session_app_id +
			" predictorOutput = " + std::to_string(it->sAB.vec[it->sAB.index].R) +
			"(deadline is " +std::to_string(it->Deadline_d)+ ") cores "
			+ std::to_string(it->sAB.vec[it->sAB.index].nCores);
		}debugMessage(debugMsg, par);

    debugMsg= " End calculate bounds ";debugMessage(debugMsg,par);


  }
