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
	std::cout<< "<debug message>: BOUND IS INVOKED"<<std::endl;
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
	std::cout<< app.V<<std::endl;
	app.currentCores_d = std::max( X0, ((int) app.V));
	std::cout<< "<debug message>: CURRENT CORES:  "<< app.currentCores_d<<std::endl;


	nCores = app.currentCores_d;

	//TODO: implementare un metodo di app che chiama invoke predictor;
	predictorOutput = atoi(invokePredictor(configuration, conn, nNodes, nCores, "*", app.datasetSize, const_cast<char*>((app.session_app_id).c_str()),
													const_cast<char*>((app.app_id).c_str()), const_cast<char*>((app.stage).c_str()), par, flagDagsim));
	debugMsg= "Predictor Output: "+ std::to_string(predictorOutput); debugMessage(debugMsg,par);

	debugMsg="Bound evaluation for" + app.session_app_id +
					 " predictorOutput = " + std::to_string(predictorOutput) +
					 "(deadline is " +std::to_string(app.Deadline_d)+ ") cores "
					 + std::to_string(nCores); debugMessage(debugMsg, par);
		app.sAB.index = 0;
		app.sAB.vec[app.sAB.index].nCores = nCores;
		app.sAB.vec[app.sAB.index].R = predictorOutput;
		app.sAB.index++;


		BTime = predictorOutput;
		//To do nCores = ceil(nCores / pointer->V ) * pointer->V;

		//printf("Calculate Bound for %s: R %d D %d\n", appId, predictorOutput, deadline);
		if (doubleCompare(predictorOutput, pointer->Deadline_d) == 1)
			while (predictorOutput > pointer->Deadline_d)
			{
				if (nCores ==0)
				{
					printf("Warning Bound (| if case): nCores is currently 0 for app. Cannot invoke Predictor\n");
					nCores= pointer->V;
					//leave the while loop
					break;
				}
				//printf("(up) time = %d Rnew =%d\n", time, BTime);

				nCores = nCores + STEP;
				predictorOutput = atof(invokePredictor(configuration, conn, nNodes, nCores, "8G", pointer->datasetSize, pointer->session_app_id, pointer->app_id, pointer->stage,par, WHOLE_DAGSIM));
				sprintf(debugMsg,"Bound evaluation for %s predictorOutput = %lf (deadline is %lf) cores %d\n",  pointer->session_app_id, predictorOutput,pointer->Deadline_d, nCores);debugMessage(debugMsg, par);

				BCores = nCores;
				BTime = predictorOutput;

				// Danilo 27/7/2017
				pointer->sAB.vec[pointer->sAB.index].nCores = nCores;
				pointer->sAB.vec[pointer->sAB.index].R = predictorOutput;
				pointer->sAB.index = (pointer->sAB.index +1) % HYP_INTERPOLATION_POINTS;



				// End Danilo

				pointer->boundIterations++;

			}
		else
			while (doubleCompare(predictorOutput, pointer->Deadline_d) == -1)
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
					nCores= pointer->V;
					//leave the while loop
					break;
				}
				predictorOutput = atof(invokePredictor(configuration, conn, nNodes, nCores, "8G", pointer->datasetSize, pointer->session_app_id, pointer->app_id, pointer->stage, par, WHOLE_DAGSIM));
				sprintf(debugMsg,"Bound evaluation for %s predictorOutput = %lf (deadline is %lf) cores %d\n",  pointer->session_app_id, predictorOutput, pointer->Deadline_d, nCores);debugMessage(debugMsg, par);

				pointer->sAB.vec[pointer->sAB.index].nCores = nCores;
				pointer->sAB.vec[pointer->sAB.index].R = predictorOutput;
				pointer->sAB.index = pointer->sAB.index % HYP_INTERPOLATION_POINTS;

				//printf("(down) time = %d Rnew =%d\n", time, BTime);
				pointer->boundIterations++;
			}
	//}
	/* Update the record with bound values */
/*
	pointer->currentCores_d = BCores;
	pointer->R_d = BTime;
	pointer->bound = BCores;
	sprintf(debugMsg,"\n\nSession_app_id %s APP_ID %s D = %lf R = %lf  bound = %d\n\n", pointer->session_app_id, pointer->session_app_id, pointer->Deadline_d, pointer->R_d, pointer->bound);debugMessage(debugMsg, par);

*/
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

  debugMsg= "findBound "+ app.session_app_id + " " + app.app_id; debugMessage(debugMsg, par);

  ///Retrieve nCores from the DB

    sprintf(statement,
                        "select num_cores_opt, num_vm_opt from %s.OPTIMIZER_CONFIGURATION_TABLE where application_id='%s' and dataset_size=%d and deadline=%lf;"
                        , db, const_cast<char*>(app.app_id.c_str()), app.datasetSize, app.Deadline_d);

    MYSQL_ROW row = executeSQL(conn, statement, par);

    if (row == NULL)
    {
      printf("Fatal error: no matches found on OPTIMIZER_CONFIGURATION_TABLE.\nCHeck that OPT_IC was run over the specific application.\n");
      exit(-1);
    }

    app.nCores_DB_d = atoi(row[0]);
    app.vm = atoi(row[1]);

    Bound(configuration, conn, app, par, WHOLE_DAGSIM);
    debugMsg="A bound for " +app.session_app_id+ " has been calculated";debugMessage(debugMsg, par);


}




/*
 * CALL IN PARALLEL FINDBOUND (OPENMP)
 */
void Bounds::calculateBounds(Batch  & app_manager, int n_threads,
                             sConfiguration &configuration, MYSQL *conn,
                             optJrParameters &par)
  {
    std::string debugMsg;
    debugMsg=" Calculate bounds for each application in parallel\n ";debugMessage(debugMsg,par);

    //sList* t_pointer[n_threads];
    MYSQL *conn2[n_threads];
    for (int i =0; i< n_threads;++i)
    {
      //t_pointer[i]=pointer;
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
          debugMsg= "Call findBound of app number " + std::to_string(j)
                  + " - called from thread" + std::to_string(ID);debugMessage(debugMsg,par);

          findBound(configuration, conn2[ID], const_cast<char*>(configuration["OptDB_dbName"].c_str()), app_manager.APPs[j], par);
        }
        ++j;
      }
    }
    debugMsg= " End calculate bounds ";debugMessage(debugMsg,par);


  }
