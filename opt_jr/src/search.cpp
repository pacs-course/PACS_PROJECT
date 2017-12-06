#include "search.hh"

#include "debugmessage.hh"
#include "utility.hh"
#include "objectiveFunction.hh"

#include <string>


/*
 * 		Name:					checkTotalNodes
 * 		Output parameters:		TBD
 * 		Description:			It checks that the total allocated nodes is still less or equal than the total number of cores available N
 *
 */

void Search::checkTotalNodes(int N, Batch &App_manager)
{
	int total = 0;

	for (auto it= App_manager.APPs.begin(); it!=App_manager.APPs.end();++it)
	{
		total+= it->currentCores_d;
		if (total > N)
		{
			printf("Fatal Error: checkTotalNodes: Total current nodes (%d) exceeds maximum nodes number (%d)\n", total, N);
			exit(-1);
		}
	}
}



/*
 * 		Name:					localSearch
 * 		Description:			Localsearch algorithm as per functional analysis
 *
 */

 void Search::localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par)
{
  std::cout <<"               hello from localsearch :) "<<std::endl;

	Application * application_i, *application_j;
	sCandidates sCandidateApproximated ;
	//char debugMsg[DEBUG_MSG];

  std::string debugMsg;
	int index = 0;
	double TotalFO;
	int how_many;
	int nCoreMov;
	int MAX_PROMISING_CONFIGURATIONS = par.get_K();
	int DELTAVM_i, DELTAVM_j;
	double DELTA_fo_App_i, DELTA_fo_App_j;
	//sCandidates  minCandidate;
	//sStatistics *firstS = NULL, *currentS = NULL;

  std::cout<< "\n***** Estimate the candidates for the predictor ******\n"<<std::endl;
	for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
	{
		debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);

		// Estimate the candidates for the predictor
		sCandidateApproximated = App_manager.approximatedLoop( how_many, par );
    std::cout<< "\n\n\n\n       finished approximatedLoop   \n\n\n\n"<<std::endl;

		if (sCandidateApproximated.empty())
		{
			// The Candidate Application is empty. No further solution enhancements possible
      debugMsg = "LocalSearch: empty Candidate Application "; debugMessage(debugMsg, par);
			break;
	  }

	// Copy the pointer to couple of application with smallest deltafo
	//minCandidate = sCandidateApproximated;
	debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";debugMessage(debugMsg, par);

//TODO: decommentare questa parte!
/*
	if (par.numberOfThreads > 0)
	{
		// Calculate in advance and in parallel the results of the predictor for each candidate
		invokePredictorOpenMP(sfirstCandidateApproximated, par, configuration);
	}*/

		// To Do: consider only the first MAX_PROMISING_CONFIGURATIONS of the Application -> DONE
		//for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)

    std::cout<<"\n\n*************************************************************************\n";
    std::cout<<"*** Consider the first MAX_PROMISING_CONFIGURATIONS of the Application ****\n";
    std::cout<<"*************************************************************************\n\n\n\n";

    std::cout << " There are "<< how_many <<" promising configurations in iteration "<< iteration <<"\n\n"<<std::endl;


    //for (int k=0;k< how_many; ++k )
		int index_pair=-1, tmp=0;
		int cores_i, cores_j;
		double DELTA_pair=0, DELTA_tmp;
		double DELTA_fo_App_i_tmp, DELTA_fo_App_j_tmp;

    for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)
		{

			checkTotalNodes(par.get_number(), App_manager);

			debugMsg=  "Browsing CandidateApproximated Application \n\n";debugMessage(debugMsg, par);

			// Consider only the first MAX_PROMISING_CONFIGURATIONS (0 value means browse the entire Application) Application members
			if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
			{
				debugMsg="LocalSearch: MAX_PROMISING_CONFIGURATIONS was reached, leaving sfirstCandidateApproximated loop";debugMessage(debugMsg, par);
				break;
			}

			application_i = it->app_i;
			application_j = it->app_j;


			debugMsg= "Comparing " + application_i->session_app_id + " with " + application_j->session_app_id; debugMessage(debugMsg, par);

			nCoreMov = std::max(application_i->V, application_j->V);

			DELTAVM_i = nCoreMov/application_i->V;
      debugMsg= "app " + application_i->session_app_id + "   DELTAVM_i: " + std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
			DELTAVM_j = nCoreMov/application_j->V;
      debugMsg= "app " + application_j->session_app_id + "    DELTAVM_j: " + std::to_string(DELTAVM_j); debugMessage(debugMsg, par);


			// Change the currentCores, but rollback later
			debugMsg = "app " + application_i->session_app_id + "     currentCores: " + std::to_string((int)application_i->currentCores_d); debugMessage(debugMsg, par);
      debugMsg = "app " + application_j->session_app_id + "     currentCores: " + std::to_string((int)application_j->currentCores_d); debugMessage(debugMsg, par);

			application_i->currentCores_d = application_i->currentCores_d + DELTAVM_i*application_i->V;
			application_j->currentCores_d = application_j->currentCores_d - DELTAVM_j*application_j->V;

			debugMsg = "After cores exchange: app " + application_i->session_app_id + " currentCores " + std::to_string( (int)application_i->currentCores_d); debugMessage(debugMsg, par);
      debugMsg = "After cores exchange: app " + application_j->session_app_id + " currentCores " + std::to_string( (int)application_j->currentCores_d); debugMessage(debugMsg, par);

			if (application_i->currentCores_d > 0 && application_j->currentCores_d > 0)
			{
				// Call object function evaluation calculated earlier
				application_i->mode= R_ALGORITHM;application_j->mode= R_ALGORITHM;
        /*
				if (par.numberOfThreads == 0) //TODO: includere openMP version
				{*/
					// No openmp
          std::cout << "\n\n\n    CALLING OBJ_FUNCTION_COMPONENT \n\n";
					DELTA_fo_App_i = ObjFun::ObjFunctionComponent(configuration, conn, *application_i, par) - application_i->baseFO;
					DELTA_fo_App_j = ObjFun::ObjFunctionComponent(configuration, conn, *application_j, par) - application_j->baseFO;
					DELTA_tmp=DELTA_fo_App_i+DELTA_fo_App_j;
					if (DELTA_tmp<DELTA_pair)
					{
						DELTA_pair=DELTA_tmp;
						index_pair=tmp;
						cores_i=application_i->currentCores_d;
						cores_j=application_j->currentCores_d;
						DELTA_fo_App_i_tmp = DELTA_fo_App_i;
						DELTA_fo_App_j_tmp = DELTA_fo_App_j;

					}

				  /*
        }
				else
				{
					// OpenMP
					DELTA_fo_App_i = sfirstCandidateApproximated->real_i - application_i->baseFO;
					DELTA_fo_App_j = sfirstCandidateApproximated->real_j - application_j->baseFO;
				}
        */
        std::cout <<"\n\n\n\n";
				 debugMsg = "app " + application_i->session_app_id + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); debugMessage(debugMsg, par);
         debugMsg = "app " + application_j->session_app_id + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); debugMessage(debugMsg, par);
				 std::cout << " TOTAL DELTA FO : "<< DELTA_tmp<<"\n\n\n\n";

			}
			// Restore previous number of cores

			application_i->currentCores_d = application_i->currentCores_d - DELTAVM_i * application_i->V;
			application_j->currentCores_d = application_j->currentCores_d + DELTAVM_j * application_j->V;
			tmp++;



		}

		//TODO: migliorare questa parte

		if(index_pair!=-1)
		{
			auto it = sCandidateApproximated.begin();
			for (int j=0; j< index_pair; j++)
			{
				it++;
			}
			it->app_i->currentCores_d = cores_i;
			it->app_i->baseFO=it->app_i->baseFO +  DELTA_fo_App_i_tmp;
			it->app_j->currentCores_d = cores_j;
			it->app_j->baseFO=it->app_j->baseFO +  DELTA_fo_App_j_tmp;
		}

/*
		if (par.get_globalFOcalculation())
		{
      std::cout << " \n\n\n\n\n\n\n\n\n\n Evaluating global F.O. "<<std::endl;

			TotalFO = ObjFun::ObjFunctionGlobal(configuration, conn, App_manager, par);

      std::cout << " \n\n\n\n GLOBAL FO: "<< TotalFO <<"\n\n\n\n\n"<<std::endl;

      /*
			sprintf(debugMsg,"\n\nGlobal obj function %lf", TotalFO);debugMessage(debugMsg, par);
			// Update Statistics
			addStatistics(&firstS, &currentS, iteration, how_many, TotalFO);
			
		}
		*/



		index++;

		// Commit the results on the Application Application and continue with another iteration
		//commitAssignment(first, minCandidate->app_i->session_app_id, minCandidate->delta_i, par); // application i
		//commitAssignment(first, minCandidate->app_j->session_app_id, minCandidate->delta_j, par); // application j
/*

NB: a cosa serve minCandidate??

    minCandidate->app_i->currentCores_d = minCandidate->app_i->currentCores_d + minCandidate->delta_i * minCandidate->app_i->V;
		minCandidate->app_j->currentCores_d = minCandidate->app_j->currentCores_d - minCandidate->delta_j * minCandidate->app_j->V;

	sprintf(debugMsg, "Information: LocalSearch: Destroy Candidate Application");debugInformational(debugMsg, par);

	// DESTROY Candidates list and prepare it for a new run
	freeCandidates(sfirstCandidateApproximated);
*/
	// TODO Modify to recalculate only FO for apps i,j (use the above copies without invoke dagSim)
	//App_manager.initialize(configuration, conn, par);

}
/*
if (par.globalFOcalculation)
{
	readStatistics(firstS, par);
	if (firstS) freeStatistics(firstS);
}
*/
}
