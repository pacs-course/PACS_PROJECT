#include "search.hh"

#include "debugmessage.hh"
#include "utility.hh"
#include "objectiveFunction.hh"
#include "statistics.hh"


#include <string>




/*
 * Name: approximatedLoop
 * Description: It estimates the objective function for each move. The candidate applications for which the move is profitable are stored in a sCandidate object
 */

sCandidates Search::approximatedLoop( Batch &App_manager, int &iteration, optJrParameters &par )
{

  std::string debugMsg;
  ObjFun OF;


	if (App_manager.APPs.empty())
	{
		printf("Error: approximatedLoop: NO Applications\n");
		exit(-1);
	}

	int nCoreMov;
	double DELTAVM_i;
	double DELTAVM_j;
	double DELTA_fo_App_i, DELTA_fo_App_j;
  sCandidates  sCandidateApproximated ;


	debugMsg= "Approximated iterated loop"; debugMessage(debugMsg, par);
  auto application_i=App_manager.APPs.begin();


  while (application_i!=App_manager.APPs.end())
  {

		auto application_j = App_manager.APPs.begin();
		while (application_j != App_manager.APPs.end())
		{
			if (application_i->get_session_app_id()!=application_j->get_session_app_id())
			{
				debugMsg="Comparing " + application_i->get_session_app_id() + " with " + application_j->get_session_app_id();debugMessage(debugMsg, par);

				nCoreMov = std::max(application_i->get_V(), application_j->get_V());

				DELTAVM_i = nCoreMov/application_i->get_V(); debugMsg = "app " + application_i->get_session_app_id() + " DELTAVM_i " +  std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
				DELTAVM_j = nCoreMov/application_j->get_V(); debugMsg = "app " + application_j->get_session_app_id() + " DELTAVM_j " +  std::to_string(DELTAVM_j); debugMessage(debugMsg, par);

				// Change the currentCores, but rollback later
				int deltaNCores_i = DELTAVM_i * application_i->get_V();
				int deltaNCores_j = DELTAVM_j * application_j->get_V();
				application_i->set_currentCores_d( application_i->get_currentCores_d() + deltaNCores_i);
				application_j->set_currentCores_d(application_j->get_currentCores_d() - deltaNCores_j);


				debugMsg= "After cores exchange: app " + application_i->get_session_app_id() + " currentCores " + std::to_string((int)application_i->get_currentCores_d());debugMessage(debugMsg, par);
        debugMsg= "After cores exchange: app " + application_j->get_session_app_id() + " currentCores " + std::to_string((int)application_j->get_currentCores_d());debugMessage(debugMsg, par);


				if (application_i->get_currentCores_d() > 0 && application_j->get_currentCores_d() > 0)
				{
					DELTA_fo_App_i = OF.ObjFunctionComponentApprox(*application_i, par) - application_i->get_baseFO();
					debugMsg = "app " + application_i->get_session_app_id() + "DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i);debugMessage(debugMsg, par);

					DELTA_fo_App_j = OF.ObjFunctionComponentApprox(*application_j, par) - application_j->get_baseFO();
          debugMsg = "app " + application_j->get_session_app_id() + "DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j);debugMessage(debugMsg, par);


          debugMsg= "\n\n  Approximated TOTAL DELTA FO: " + std::to_string(DELTA_fo_App_i + DELTA_fo_App_j) + "   \n\n "; debugMessage(debugMsg, par);


					if ((DELTA_fo_App_i + DELTA_fo_App_j < 0))
					{
            debugMsg= "\n\nAdding candidate  \n\n "; debugMessage(debugMsg, par);
						sCandidateApproximated.addCandidate( *application_i ,
									                               *application_j ,
									                                application_i->get_currentCores_d(),
									                                application_j->get_currentCores_d(),
									                                DELTA_fo_App_i + DELTA_fo_App_j,
									                                DELTAVM_i,
									                                DELTAVM_j) ;

					}


				}

				application_i->set_currentCores_d( application_i->get_currentCores_d() - DELTAVM_i*application_i->get_V());
				application_j->set_currentCores_d( application_j->get_currentCores_d() + DELTAVM_j*application_j->get_V());
			}
		application_j++;
		}
  application_i++;
	}



  iteration= sCandidateApproximated.cand.size();

	return sCandidateApproximated;

}





/*
 * 		Name:					checkTotalNodes
 * 		Description:			It checks that the total allocated nodes is still less or equal than the total number of cores available N
 */

void Search::checkTotalNodes(int N, Batch &App_manager)
{
	int total = 0;

	for (auto it= App_manager.APPs.begin(); it!=App_manager.APPs.end();++it)
	{
		total+= it->get_currentCores_d();
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

 void Search::localSearch(sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)
 {

   std::string debugMsg;
   sCandidates sCandidateApproximated ;
   double TotalFO;
   int how_many;
   int MAX_PROMISING_CONFIGURATIONS = par.get_K();
   double DELTA_fo_App_i, DELTA_fo_App_j;
   sStatistics statistics;
   int index_pair=-1, index=0; //Those values are useful to detect the best pair
   double DELTA_pair=0, DELTA_tmp;
   ObjFun OF;

   debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; debugMessage(debugMsg, par);

   //START THE ITERATION LOOP
   for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
   {
     checkTotalNodes(par.get_number(), App_manager);
     debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);


     /*
     *   Estimate the candidates for the predictor
     */
     sCandidateApproximated = approximatedLoop( App_manager, how_many, par );
     debugMsg= "\n\n\n\n       finished approximatedLoop   \n\n\n\n"; debugMessage(debugMsg, par);

     if (sCandidateApproximated.cand.empty())
     {
       // The Candidate Application is empty. No further solution enhancements possible
       debugMsg = "LocalSearch: empty Candidate Application "; debugMessage(debugMsg, par);
       break;
     }

     debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";debugMessage(debugMsg, par);


     debugMsg = "\n\n*******************************************************************\n";
     debugMsg +="*** Consider the first MAX_PROMISING_CONFIGURATIONS of the Application ****\n";
     debugMsg +="*******************************************************************\n"; debugMessage(debugMsg,par);

     debugMsg = " There are " + std::to_string(how_many) +" promising configurations in iteration " + std::to_string(iteration) + "\n\n"; debugMessage(debugMsg,par);

     /*
     *   Evaluate objective function for most promising pairs (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
     */

     if(par.get_numberOfThreads()==0)
     {
       // Calculate in advance the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
       sCandidateApproximated.invokePredictorSeq( conn, par, configuration);
     }
     else
     {
       // Calculate in advance and in parallel the results of the predictor FOR EACH candidate (TODO:to be changed)
       sCandidateApproximated.invokePredictorOpenMP( par, configuration);
     }

     /*
     *  Find the best pair with deltaFO<0 (if it exists)
     */



     //Those values are useful to detect the best pair
     index_pair=-1;
     index=0;
     DELTA_pair=0;


     for (auto it = sCandidateApproximated.cand.begin(); it != sCandidateApproximated.cand.end(); it++)
     {
       if(index>0 && index==MAX_PROMISING_CONFIGURATIONS)//This is done because in invokePredictor* only the first MAX_PROMISING_CONFIGURATIONS are evaluated; index=0 means par.get_k()==0 i.e. all the pairs are explored
       {
         break;
       }

       if (it->app_i.get_currentCores_d() > 0 && it->app_j.get_currentCores_d() > 0) // For those pairs, object function evaluation calculated earlier
       {

         DELTA_fo_App_i = it->real_i - it->app_i.get_baseFO();
         DELTA_fo_App_j = it->real_j - it->app_j.get_baseFO();

         DELTA_tmp=DELTA_fo_App_i+DELTA_fo_App_j;

         if (DELTA_tmp<DELTA_pair)
         {
           DELTA_pair=DELTA_tmp;
           index_pair=index;
         }

         debugMsg = "app " + it->app_i.get_session_app_id() + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); debugMessage(debugMsg, par);
         debugMsg = "app " + it->app_j.get_session_app_id() + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); debugMessage(debugMsg, par);
         debugMsg= "\n      TOTAL DELTA FO : " +  std::to_string(DELTA_tmp) + "\n\n"; debugMessage(debugMsg, par);
       }

       index++;

     }


   /*
    *  Find (if any) the corresponding applications in batch and modify them
    */

     if(index_pair!=-1)
     {
       auto it = sCandidateApproximated.cand.begin();
       for (int j=0; j< index_pair; j++)
       {
         it++;
       }

       for (auto &elem : App_manager.APPs)
       {
         if (it->app_i.get_app_id()==elem.get_app_id() && it->app_i.get_session_app_id()==elem.get_session_app_id())
         {
           elem.set_currentCores_d( it->newCoreAssignment_i);
           elem.set_baseFO( it->real_i);
         }
         if (it->app_j.get_app_id()==elem.get_app_id() && it->app_j.get_session_app_id()==elem.get_session_app_id())
         {
           elem.set_currentCores_d( it->newCoreAssignment_j);
           elem.set_baseFO( it->real_j);
         }
       }
     }


     checkTotalNodes(par.get_number(), App_manager);


     if (par.get_globalFOcalculation())
     {
       TotalFO = OF.ObjFunctionGlobal(configuration, conn, App_manager, par);
       std::cout << "\n At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<"\n"<<std::endl;
       addStatistics(statistics, iteration, how_many, TotalFO); // Update Statistics
     }

     if (index_pair==-1)
     {
       break; // It means that even there are promising configuration, but they are not convenient
     }

   }



   if (par.get_globalFOcalculation())
   {
     readStatistics(statistics, par);
   }

 }

 void Search::writeResults(MYSQL *conn, char * dbName,  optJrParameters &par)
 {
   App_manager.writeResults(conn, dbName, par);
 }





 void Search::print_solution()
 {
   std::cout<< App_manager.show_solution();

 }
