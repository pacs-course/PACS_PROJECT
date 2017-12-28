#include "bounds.hh"


#include "db.hh"
#include "invoke_predictor.hh"
#include <omp.h>
#include <string>
#include <math.h>




/**
bound calculates the bound given a certain deadline and number of cores. Predictor method is invoked until an upper bound,
consisting of the number of nodes, is found (once that the time calculated by the predictor, a rollback is performed to
return the last "safe" number of core and time).
*/
void  Bounds::bound(Configuration &configuration, MYSQL *conn, Application &app, Opt_jr_parameters &par, int flagDagsim)
{
	double predictorOutput;
  std::string debugMsg;
	double BTime = 0;
	int BCores = 0;
	int STEP = app.get_V();
	int nCores;
	int nNodes = 1;

	app.set_currentCores_d(app.get_nCores_DB_d());


	int X0 = ((int) ( app.get_currentCores_d() / app.get_V()) ) * app.get_V();
	app.set_currentCores_d( std::max( X0, ((int) app.get_V())));


	nCores = app.get_currentCores_d();


	/*
	 *   First evaluation, with data guessed by findbound
	 */

	predictorOutput = atoi(invoke_predictor(configuration, conn, nNodes, nCores, (char*)"*", app.get_dataset_size(), const_cast<char*>((app.get_session_app_id()).c_str()),
													const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par, flagDagsim));


	debugMsg="Bound evaluation for " + app.get_session_app_id() +
					 " (app_ID: " + app.get_app_id() +  ") " +
					 " predictorOutput = " + std::to_string(predictorOutput) +
					 "(deadline is " +std::to_string(app.get_Deadline_d())+ ") cores "
					 + std::to_string(nCores); par.debug_message(debugMsg);


		app.compute_alpha_beta(nCores, predictorOutput);//first call


		BTime = predictorOutput;

		/*
		 *   START THE HILL CLIMBING
		 */
		if (double_compare(predictorOutput, app.get_Deadline_d()) == 1)
			while (predictorOutput > app.get_Deadline_d())
			{
				if (nCores ==0)
				{
					printf("Warning Bound (| if case): nCores is currently 0 for app. Cannot invoke Predictor\n");
					nCores= app.get_V();
					//leave the while loop
					break;
				}

				nCores = nCores + STEP; //add the cores
				predictorOutput = atof(invoke_predictor(configuration, conn, nNodes, nCores, (char*)"8G", app.get_dataset_size(), const_cast<char*>((app.get_session_app_id()).c_str()),
																const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par, WHOLE_DAGSIM));

				debugMsg="Bound evaluation for " + app.get_session_app_id() +
								 " predictorOutput = " + std::to_string(predictorOutput) +
								 "(deadline is " +std::to_string(app.get_Deadline_d())+ ") cores "
								 + std::to_string(nCores); par.debug_message(debugMsg);

				BCores = nCores;
				BTime = predictorOutput;

				app.compute_alpha_beta(nCores, predictorOutput);

			}

		else
			while (double_compare(predictorOutput, app.get_Deadline_d()) == -1)//while predictorOutput< deadline
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
					nCores= app.get_V();
					//leave the while loop
					break;
				}

				predictorOutput = atof(invoke_predictor(configuration, conn, nNodes, nCores, (char*)"8G", app.get_dataset_size(), const_cast<char*>((app.get_session_app_id()).c_str()),
																const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par, WHOLE_DAGSIM));

				debugMsg="Bound evaluation for" + app.get_session_app_id() +
								 " predictorOutput = " + std::to_string(predictorOutput) +
								 "(deadline is " +std::to_string(app.get_Deadline_d())+ ") cores "
								 + std::to_string(nCores); par.debug_message(debugMsg);

				app.compute_alpha_beta(nCores, predictorOutput);

			}

	/* Update the record with bound values */

	app.set_currentCores_d( BCores);
	app.set_R_d( BTime);
	app.set_bound( BCores);
	debugMsg="\n\nSession_app_id : " + app.get_session_app_id() + " , APP_ID: " + app.get_app_id() +
					 ", D = "+ std::to_string(app.get_Deadline_d()) + ", R =" + std::to_string(app.get_R_d())+
					 ", bound = "+ std::to_string(app.get_bound()) + "\n\n"; par.debug_message(debugMsg);


}



/**
Initially, find_bound queries the lookup table to find the number of cores, calculated by OPT_IC earlier,
given a deadline, an application id and a dataset size.
Secondly, it invokes the bound function.
*/
void Bounds::find_bound(Configuration &configuration, MYSQL *conn, char* db,  Application &app, Opt_jr_parameters &par)
{
  std::string debugMsg;
  char statement[256];




    sprintf(statement,
                        "select num_cores_opt, num_vm_opt from %s.OPTIMIZER_CONFIGURATION_TABLE where application_id='%s' and dataset_size=%d and deadline=%lf;"
                        , db, const_cast<char*>(app.get_app_id().c_str()), app.get_dataset_size(), app.get_Deadline_d());
		debugMsg= "From findbound executing SQL STATEMENT below for app "+app.get_app_id(); par.debug_message(debugMsg);
    MYSQL_ROW row = executeSQL(conn, statement, par);

    if (row == NULL)
    {
      printf("Fatal error: no matches found on OPTIMIZER_CONFIGURATION_TABLE.\nCHeck that OPT_IC was run over the specific application.\n");
      exit(-1);
    }

    app.set_nCores_DB_d( atoi(row[0]));
    app.set_vm(atoi(row[1]));
		debugMsg=" From findbound a first estimate on ncores:"+  std::to_string(app.get_nCores_DB_d())
							+" and VM: "+ std::to_string(app.get_vm()) + " for application "+app.get_app_id() ;par.debug_message(debugMsg);



    bound(configuration, conn, app, par,WHOLE_DAGSIM);
    debugMsg="A bound for " + app.get_session_app_id() + "  (app_id: " + app.get_app_id() + ") has been calculated";par.debug_message(debugMsg);


}

/*
 * CALL IN PARALLEL FINDBOUND (OPENMP)
 */
void Bounds::calculate_bounds( Configuration &configuration, MYSQL *conn,
                              Opt_jr_parameters &par)
  {
		int n_threads = par.get_number_of_threads();
    std::string debugMsg;
		if (n_threads>0)
		{
    debugMsg=" Calculate bounds for each application in parallel with "+ std::to_string(n_threads)+" threads (using openMP) \n" ;par.debug_message(debugMsg);

    MYSQL *conn2[n_threads];
    for (int i =0; i< n_threads;++i)
    {
      conn2[i]=DBopen(
        const_cast<char*>(configuration["OptDB_IP"].c_str()),
				const_cast<char*>(configuration["DB_port"].c_str()),
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
      for (auto it=app_manager.get_begin(); it !=app_manager.get_end(); it++)
      {
        int pos=j%n_threads;
        if(pos==ID)
        {
          debugMsg= "Call find_bound of app " + it->get_app_id()
                    + " from thread " + std::to_string(ID); par.debug_message(debugMsg);

          find_bound(configuration, conn2[ID], const_cast<char*>(configuration["OptDB_dbName"].c_str()), *it , par);
        }
        ++j;
      }
    }

		for (int i = 0; i < n_threads;++i)
	    DBclose(conn2[i]);

	}
	else
	{

		debugMsg=" Calculate bounds for each application (sequencial version) \n" ;par.debug_message(debugMsg);
		for (auto it=app_manager.get_begin(); it !=app_manager.get_end(); it++)
				find_bound(configuration, conn, const_cast<char*>(configuration["OptDB_dbName"].c_str()), *it, par);

	}



    debugMsg= " End calculate bounds ";par.debug_message(debugMsg);


  }
