#include "search.hh"

#include <string>


/*
 * 		Name:					localSearch
 * 		Description:			Localsearch algorithm as per functional analysis
 *
 */
 void Search::localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par)
{
  std::cout <<"               hello from localsearch :) "<<std::endl;

	//sApplication * application_i, *application_j;
	//sCandidates *sfirstCandidateApproximated = NULL;
	//char debugMsg[DEBUG_MSG];

  std::string debugMsg;
	int index = 0;
	double TotalFO;
	int how_many;
	int nCoreMov;
	int MAX_PROMISING_CONFIGURATIONS = par.get_K();
	int DELTAVM_i, DELTAVM_j;
	double DELTA_fo_App_i, DELTA_fo_App_j;
	//sCandidates * minCandidate;
	//sStatistics *firstS = NULL, *currentS = NULL;


	for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
	{
		debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);

    /*
		// Estimate the candidates for the predictor
		sfirstCandidateApproximated = approximatedLoop(first, &how_many, par );
		if (sfirstCandidateApproximated ==NULL)
		{
			// The Candidate Application is empty. No further solution enhancements possible
			debugInformational("LocalSearch: empty Candidate Application", par);

			break;
	}

	// Copy the pointer to couple of application with smallest deltafo
	minCandidate = sfirstCandidateApproximated;
	sprintf(debugMsg, " Ex-iteration loop");debugBanner(debugMsg, par);

	if (par.numberOfThreads > 0)
	{
		// Calculate in advance and in parallel the results of the predictor for each candidate
		invokePredictorOpenMP(sfirstCandidateApproximated, par, configuration);
	}

		// To Do: consider only the first MAX_PROMISING_CONFIGURATIONS of the Application -> DONE
		while (sfirstCandidateApproximated != NULL)
		{
			checkTotalNodes(par.number, first);

			sprintf(debugMsg, "Browsing CandidateApproximated Application");debugBanner(debugMsg, par);
			// Consider only the first MAX_PROMISING_CONFIGURATIONS (0 value means browse the entire Application) Application members
			if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
			{
				sprintf(debugMsg,"LocalSearch: MAX_PROMISING_CONFIGURATIONS was reached, leaving sfirstCandidateApproximated loop");debugMessage(debugMsg, par);
				break;
			}

			application_i = sfirstCandidateApproximated->app_i;
			application_j = sfirstCandidateApproximated->app_j;

			sprintf(debugMsg, "\n\nComparing %s with %s", application_i->session_app_id, application_j->session_app_id);

			nCoreMov = max(application_i->V, application_j->V);

			DELTAVM_i = nCoreMov/application_i->V;sprintf(debugMsg, "app %s DELTAVM_i %d", application_i->session_app_id, DELTAVM_i);debugMessage(debugMsg, par);
			DELTAVM_j = nCoreMov/application_j->V;sprintf(debugMsg, "app %s DELTAVM_j %d", application_j->session_app_id, DELTAVM_j);debugMessage(debugMsg, par);

			// Change the currentCores, but rollback later
			sprintf(debugMsg,"app %s currentCores %d", application_i->session_app_id, (int)application_i->currentCores_d);debugMessage(debugMsg, par);
			sprintf(debugMsg,"app %s currentCores %d", application_j->session_app_id, (int)application_j->currentCores_d);debugMessage(debugMsg, par);

			application_i->currentCores_d = application_i->currentCores_d + DELTAVM_i*application_i->V;
			application_j->currentCores_d = application_j->currentCores_d - DELTAVM_j*application_j->V;

			sprintf(debugMsg,"After cores exchange: app %s currentCores %d", application_i->session_app_id, (int)application_i->currentCores_d);debugMessage(debugMsg, par);
			sprintf(debugMsg,"After cores exchange: app %s currentCores %d", application_j->session_app_id, (int)application_j->currentCores_d);debugMessage(debugMsg, par);

			if (application_i->currentCores_d > 0 && application_j->currentCores_d > 0)
			{
				// Call object function evaluation calculated earlier
				application_i->mode= R_ALGORITHM;application_j->mode= R_ALGORITHM;

				if (par.numberOfThreads == 0)
				{
					// No openmp
					DELTA_fo_App_i = ObjFunctionComponent(configuration, conn, application_i, par) - application_i->baseFO;
					DELTA_fo_App_j = ObjFunctionComponent(configuration, conn, application_j, par) - application_j->baseFO;
				}
				else
				{
					// OpenMP
					DELTA_fo_App_i = sfirstCandidateApproximated->real_i - application_i->baseFO;
					DELTA_fo_App_j = sfirstCandidateApproximated->real_j - application_j->baseFO;
				}
				 sprintf(debugMsg, "app %s DELTA_fo_App_i %lf",application_i->session_app_id, DELTA_fo_App_i);debugMessage(debugMsg, par);
				 sprintf(debugMsg, "app %s DELTA_fo_App_j %lf",application_j->session_app_id, DELTA_fo_App_j);debugMessage(debugMsg, par);
			}
			// Restore previous number of cores
			application_i->currentCores_d = application_i->currentCores_d - DELTAVM_i * application_i->V;
			application_j->currentCores_d = application_j->currentCores_d + DELTAVM_j * application_j->V;

			sfirstCandidateApproximated = sfirstCandidateApproximated->next;
		}
		if (par.globalFOcalculation)
		{
			TotalFO = ObjFunctionGlobal(configuration, conn, first, par);
			sprintf(debugMsg,"\n\nGlobal obj function %lf", TotalFO);debugMessage(debugMsg, par);
			// Update Statistics
			addStatistics(&firstS, &currentS, iteration, how_many, TotalFO);
		}
		index++;

		// Commit the results on the Application Application and continue with another iteration
		//commitAssignment(first, minCandidate->app_i->session_app_id, minCandidate->delta_i, par); // application i
		//commitAssignment(first, minCandidate->app_j->session_app_id, minCandidate->delta_j, par); // application j
		minCandidate->app_i->currentCores_d = minCandidate->app_i->currentCores_d + minCandidate->delta_i * minCandidate->app_i->V;
		minCandidate->app_j->currentCores_d = minCandidate->app_j->currentCores_d - minCandidate->delta_j * minCandidate->app_j->V;

	sprintf(debugMsg, "Information: LocalSearch: Destroy Candidate Application");debugInformational(debugMsg, par);

	// DESTROY Candidates list and prepare it for a new run
	freeCandidates(sfirstCandidateApproximated);

	// TODO Modify to recalculate only FO for apps i,j (use the above copies without invoke dagSim)
	initialize(configuration, conn, first, par);
*/
}
/*
if (par.globalFOcalculation)
{
	readStatistics(firstS, par);
	if (firstS) freeStatistics(firstS);
}
*/
}
