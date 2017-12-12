#include "search.hh"

#include "debugmessage.hh"
#include "utility.hh"
#include "objectiveFunction.hh"
#include "statistics.hh"


#include <string>




/*
 * Name: approximatedLoop
 * Description: It estimates the objective function for each move. The candidate applications for which the move is profitable is stored in a sCandidate object
 */

sCandidates Search::approximatedLoop( Batch &App_manager, int &iteration, optJrParameters &par )
{

  std::string debugMsg;


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
			if (application_i->session_app_id!=application_j->session_app_id)
			{
				debugMsg="Comparing " + application_i->session_app_id + " with " + application_j->session_app_id;debugMessage(debugMsg, par);

				nCoreMov = std::max(application_i->V, application_j->V);

				DELTAVM_i = nCoreMov/application_i->V; debugMsg = "app " + application_i->session_app_id + " DELTAVM_i " +  std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
				DELTAVM_j = nCoreMov/application_j->V; debugMsg = "app " + application_j->session_app_id + " DELTAVM_j " +  std::to_string(DELTAVM_j); debugMessage(debugMsg, par);

				// Change the currentCores, but rollback later
				int deltaNCores_i = DELTAVM_i * application_i->V;
				int deltaNCores_j = DELTAVM_j * application_j->V;
				application_i->currentCores_d = application_i->currentCores_d + deltaNCores_i;
				application_j->currentCores_d = application_j->currentCores_d - deltaNCores_j;


				debugMsg= "After cores exchange: app " + application_i->session_app_id + " currentCores " + std::to_string((int)application_i->currentCores_d);debugMessage(debugMsg, par);
        debugMsg= "After cores exchange: app " + application_j->session_app_id + " currentCores " + std::to_string((int)application_j->currentCores_d);debugMessage(debugMsg, par);


				if (application_i->currentCores_d > 0 && application_j->currentCores_d > 0)
				{
					DELTA_fo_App_i = ObjFun::ObjFunctionComponentApprox(*application_i, par) - application_i->baseFO;
					debugMsg = "app " + application_i->session_app_id + "DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i);debugMessage(debugMsg, par);

					DELTA_fo_App_j = ObjFun::ObjFunctionComponentApprox(*application_j, par) - application_j->baseFO;
          debugMsg = "app " + application_j->session_app_id + "DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j);debugMessage(debugMsg, par);


          debugMsg= "\n\n  Approximated TOTAL DELTA FO: " + std::to_string(DELTA_fo_App_i + DELTA_fo_App_j) + "   \n\n "; debugMessage(debugMsg, par);


					if ((DELTA_fo_App_i + DELTA_fo_App_j < 0))
					{
            debugMsg= "\n\nAdding candidate  \n\n "; debugMessage(debugMsg, par);
						addCandidate(sCandidateApproximated,
									*application_i ,
									*application_j ,
									application_i->currentCores_d,
									application_j->currentCores_d,
									DELTA_fo_App_i + DELTA_fo_App_j,
									DELTAVM_i,
									DELTAVM_j) ;

					}


				}

				application_i->currentCores_d = application_i->currentCores_d - DELTAVM_i*application_i->V;
				application_j->currentCores_d = application_j->currentCores_d + DELTAVM_j*application_j->V;
			}
		application_j++;
		}
  application_i++;
	}



  iteration= sCandidateApproximated.size();

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

     if (par.get_numberOfThreads() > 0) // openMP
     {
       openMP_localSearch(configuration, conn, App_manager,  par );
     }
     else
     {
       sequencial_localSearch(configuration, conn, App_manager,  par );
     }

}

/*
Sequencial localSearch
*/


void Search::sequencial_localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par)
{
  //Application * application_i, *application_j;
  sCandidates sCandidateApproximated ;

  std::string debugMsg;

  int index = 0;
  double TotalFO;
  int how_many;
  int nCoreMov;
  int MAX_PROMISING_CONFIGURATIONS = par.get_K();
  int DELTAVM_i, DELTAVM_j;
  double DELTA_fo_App_i, DELTA_fo_App_j;
  //sCandidates  minCandidate;
  sStatistics statistics;

  debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; debugMessage(debugMsg, par);

  //START THE ITERATION LOOP
  for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
  {
    debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);

    // Estimate the candidates for the predictor
    sCandidateApproximated = approximatedLoop( App_manager, how_many, par );
    debugMsg= "\n\n\n\n       finished approximatedLoop   \n\n\n\n"; debugMessage(debugMsg, par);

    if (sCandidateApproximated.empty())
    {
      // The Candidate Application is empty. No further solution enhancements possible
      debugMsg = "LocalSearch: empty Candidate Application "; debugMessage(debugMsg, par);
      break;
    }

    // Copy the pointer to couple of application with smallest deltafo
    //minCandidate = sCandidateApproximated;
    debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";debugMessage(debugMsg, par);


    debugMsg = "\n\n*******************************************************************\n";
    debugMsg +="*** Consider the first MAX_PROMISING_CONFIGURATIONS of the Application ****\n";
    debugMsg +="*******************************************************************\n"; debugMessage(debugMsg,par);

    debugMsg = " There are " + std::to_string(how_many) +" promising configurations in iteration " + std::to_string(iteration) + "\n\n"; debugMessage(debugMsg,par);



    int index_pair=-1, tmp=0;
    int cores_i, cores_j;
    double DELTA_pair=0, DELTA_tmp;
    double DELTA_fo_App_i_tmp, DELTA_fo_App_j_tmp;
    //std::string sID_i, sID_j, ID_i, ID_j;

    checkTotalNodes(par.get_number(), App_manager);

    for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)
    {

      debugMsg=  "Browsing CandidateApproximated Application \n\n";debugMessage(debugMsg, par);

      // Consider only the first MAX_PROMISING_CONFIGURATIONS (0 value means browse the entire Application) Application members
      if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
      {
        debugMsg="LocalSearch: MAX_PROMISING_CONFIGURATIONS was reached, leaving sfirstCandidateApproximated loop";debugMessage(debugMsg, par);
        break;
      }

      //application_i = it->app_i;
      //application_j = it->app_j;


      debugMsg= "Comparing " + it->app_i.session_app_id + " with " + it->app_j.session_app_id; debugMessage(debugMsg, par);

      nCoreMov = std::max(it->app_i.V, it->app_j.V);

      DELTAVM_i = nCoreMov/it->app_i.V;
      debugMsg= "app " + it->app_i.session_app_id + "   DELTAVM_i: " + std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
      DELTAVM_j = nCoreMov/it->app_j.V;
      debugMsg= "app " + it->app_j.session_app_id + "    DELTAVM_j: " + std::to_string(DELTAVM_j); debugMessage(debugMsg, par);


      // Change the currentCores, but rollback later
      debugMsg = "app " + it->app_i.session_app_id + "     currentCores: " + std::to_string((int)it->app_i.currentCores_d); debugMessage(debugMsg, par);
      debugMsg = "app " + it->app_j.session_app_id + "     currentCores: " + std::to_string((int)it->app_j.currentCores_d); debugMessage(debugMsg, par);

      it->app_i.currentCores_d = it->app_i.currentCores_d + DELTAVM_i*it->app_i.V;
      it->app_j.currentCores_d = it->app_j.currentCores_d - DELTAVM_j*it->app_j.V;

      debugMsg = "After cores exchange: app " + it->app_i.session_app_id + " currentCores " + std::to_string( (int)it->app_i.currentCores_d); debugMessage(debugMsg, par);
      debugMsg = "After cores exchange: app " + it->app_j.session_app_id + " currentCores " + std::to_string( (int)it->app_j.currentCores_d); debugMessage(debugMsg, par);

      if (it->app_i.currentCores_d > 0 && it->app_j.currentCores_d > 0)
      {

        it->app_i.mode= R_ALGORITHM; it->app_j.mode= R_ALGORITHM;


          // No openmp
          debugMsg =  " CALLING OBJ_FUNCTION_COMPONENT \n\n"; debugMessage(debugMsg, par);
          DELTA_fo_App_i = ObjFun::ObjFunctionComponent(configuration, conn, it->app_i, par) - it->app_i.baseFO;
          DELTA_fo_App_j = ObjFun::ObjFunctionComponent(configuration, conn, it->app_j, par) - it->app_j.baseFO;




        DELTA_tmp=DELTA_fo_App_i+DELTA_fo_App_j;




        if (DELTA_tmp<DELTA_pair)
        {
          DELTA_pair=DELTA_tmp;
          index_pair=tmp;
          cores_i=it->app_i.currentCores_d;
          cores_j=it->app_j.currentCores_d;
          DELTA_fo_App_i_tmp = DELTA_fo_App_i;
          DELTA_fo_App_j_tmp = DELTA_fo_App_j;
        }

        debugMsg = "app " + it->app_i.session_app_id + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); debugMessage(debugMsg, par);
        debugMsg = "app " + it->app_j.session_app_id + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); debugMessage(debugMsg, par);
        debugMsg= "\n      TOTAL DELTA FO : " +  std::to_string(DELTA_tmp) + "\n\n"; debugMessage(debugMsg, par);

      }
      // Restore previous number of cores

    //  application_i->currentCores_d = application_i->currentCores_d - DELTAVM_i * application_i->V;
      //it->app_j->currentCores_d = it->app_j->currentCores_d + DELTAVM_j * it->app_j->V;
      tmp++;



    }



    if(index_pair!=-1)
    {
      auto it = sCandidateApproximated.begin();
      for (int j=0; j< index_pair; j++)
      {
        it++;
      }
      // potrei andare per copia, scorrere app in batch e trovare quelle con session app_id e ID selezionati:

        for (auto &elem : App_manager.APPs)
        {
          if (it->app_i.app_id==elem.app_id && it->app_i.session_app_id==elem.session_app_id)
          {
            elem.currentCores_d = cores_i;
            elem.baseFO=it->app_i.baseFO +  DELTA_fo_App_i_tmp;
          }
          if (it->app_j.app_id==elem.app_id && it->app_j.session_app_id==elem.session_app_id)
          {
            elem.currentCores_d = cores_j;
            elem.baseFO=it->app_i.baseFO +  DELTA_fo_App_j_tmp;
          }

        }

      /*
      it->app_i->currentCores_d = cores_i;
      it->app_i->baseFO=it->app_i->baseFO +  DELTA_fo_App_i_tmp;
      it->app_j->currentCores_d = cores_j;
      it->app_j->baseFO=it->app_j->baseFO +  DELTA_fo_App_j_tmp;
      */
    }


    if (par.get_globalFOcalculation())
    {

      TotalFO = ObjFun::ObjFunctionGlobal(configuration, conn, App_manager, par);

      std::cout << "\n At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<"\n"<<std::endl;

      // Update Statistics
      addStatistics(statistics, iteration, how_many, TotalFO);

    }

    index++;
  }

  if (par.get_globalFOcalculation())
  {
    readStatistics(statistics, par);
  }
}



/*
 Parallel localSearch
*/

void Search::openMP_localSearch(sConfiguration &configuration, MYSQL *conn, Batch &App_manager , optJrParameters &par)
{
  //Application * application_i, *application_j;
  sCandidates sCandidateApproximated ;

  std::string debugMsg;

  int index = 0;
  double TotalFO;
  int how_many;
  int nCoreMov;
  //int MAX_PROMISING_CONFIGURATIONS = par.get_K();
  int DELTAVM_i, DELTAVM_j;
  double DELTA_fo_App_i, DELTA_fo_App_j;
  //sCandidates  minCandidate;
  sStatistics statistics;

  debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; debugMessage(debugMsg, par);

  //START THE ITERATION LOOP
  for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
  {
    debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);

    // Estimate the candidates for the predictor
    sCandidateApproximated = approximatedLoop( App_manager, how_many, par );
    debugMsg= "\n\n\n\n       finished approximatedLoop   \n\n\n\n"; debugMessage(debugMsg, par);

    if (sCandidateApproximated.empty())
    {
      // The Candidate Application is empty. No further solution enhancements possible
      debugMsg = "LocalSearch: empty Candidate Application "; debugMessage(debugMsg, par);
      break;
    }

    // Copy the pointer to couple of application with smallest deltafo
    //minCandidate = sCandidateApproximated;
    debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";debugMessage(debugMsg, par);


    debugMsg = "\n\n*******************************************************************\n";
    debugMsg +="*** Consider the first MAX_PROMISING_CONFIGURATIONS of the Application ****\n";
    debugMsg +="*******************************************************************\n"; debugMessage(debugMsg,par);

    debugMsg = " There are " + std::to_string(how_many) +" promising configurations in iteration " + std::to_string(iteration) + "\n\n"; debugMessage(debugMsg,par);



    int index_pair=-1, tmp=0;
    int cores_i, cores_j;
    double DELTA_pair=0, DELTA_tmp;
    double DELTA_fo_App_i_tmp, DELTA_fo_App_j_tmp;

    checkTotalNodes(par.get_number(), App_manager);

    // Change the currentCores, but rollback later
    for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)
    {

      //application_i = it->app_i;
      //application_j = it->app_j;
      nCoreMov = std::max(it->app_i.V, it->app_j.V);

      DELTAVM_i = nCoreMov/it->app_i.V;
      DELTAVM_j = nCoreMov/it->app_j.V;

      it->app_i.currentCores_d = it->app_i.currentCores_d + DELTAVM_i*it->app_i.V;
      it->app_j.currentCores_d = it->app_j.currentCores_d - DELTAVM_j*it->app_j.V;
    }


    // Calculate in advance and in parallel the results of the predictor FOR EACH candidate
    invokePredictorOpenMP(sCandidateApproximated, par, configuration);

    // find the best pair <0 (it it exists)
    for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)
    {
      // OpenMP:  object function just computed

      if (it->app_i.currentCores_d > 0 && it->app_j.currentCores_d > 0)
      {


        // OpenMP:  object function evaluation calculated earlier
        DELTA_fo_App_i = it->real_i - it->app_i.baseFO;
        DELTA_fo_App_j = it->real_j - it->app_j.baseFO;



        DELTA_tmp=DELTA_fo_App_i+DELTA_fo_App_j;




        if (DELTA_tmp<DELTA_pair)
        {
          DELTA_pair=DELTA_tmp;
          index_pair=tmp;
          cores_i=it->app_i.currentCores_d;
          cores_j=it->app_j.currentCores_d;
          DELTA_fo_App_i_tmp = DELTA_fo_App_i;
          DELTA_fo_App_j_tmp = DELTA_fo_App_j;
        }

        debugMsg = "app " + it->app_i.session_app_id + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); debugMessage(debugMsg, par);
        debugMsg = "app " + it->app_j.session_app_id + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); debugMessage(debugMsg, par);
        debugMsg= "\n      TOTAL DELTA FO : " +  std::to_string(DELTA_tmp) + "\n\n"; debugMessage(debugMsg, par);

      }
      tmp++;
    }

    // Restore previous number of cores
    /*
    for (auto it = sCandidateApproximated.begin(); it != sCandidateApproximated.end(); it++)
    {
      //application_i = it->app_i;
      //application_j = it->app_j;

      nCoreMov = std::max(it->app_i.V, it->app_j.V);

      DELTAVM_i = nCoreMov/it->app_i.V;
      DELTAVM_j = nCoreMov/it->app_j.V;


      it->app_i.currentCores_d = it->app_i.currentCores_d - DELTAVM_i * it->app_i.V;
      it->app_j.currentCores_d = it->app_j.currentCores_d + DELTAVM_j * it->app_j.V;
    }
    */

    if(index_pair!=-1)
    {
      auto it = sCandidateApproximated.begin();
      for (int j=0; j< index_pair; j++)
      {
        it++;
      }
      // potrei andare per copia, scorrere app in batch e trovare quelle con session app_id e ID selezionati:

        for (auto &elem : App_manager.APPs)
        {
          if (it->app_i.app_id==elem.app_id && it->app_i.session_app_id==elem.session_app_id)
          {
            elem.currentCores_d = cores_i;
            elem.baseFO=elem.baseFO +  DELTA_fo_App_i_tmp;
          }
          if (it->app_j.app_id==elem.app_id && it->app_j.session_app_id==elem.session_app_id)
          {
            elem.currentCores_d = cores_j;
            elem.baseFO=elem.baseFO +  DELTA_fo_App_j_tmp;
          }

        }

      /*
      it->app_i->currentCores_d = cores_i;
      it->app_i->baseFO=it->app_i->baseFO +  DELTA_fo_App_i_tmp;
      it->app_j->currentCores_d = cores_j;
      it->app_j->baseFO=it->app_j->baseFO +  DELTA_fo_App_j_tmp;
      */
    }


    if (par.get_globalFOcalculation())
    {


      TotalFO = ObjFun::ObjFunctionGlobal(configuration, conn, App_manager, par);

      std::cout << "\n At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<"\n"<<std::endl;


      // Update Statistics
      addStatistics(statistics, iteration, how_many, TotalFO);

    }

    index++;

  }

  if (par.get_globalFOcalculation())
  {
    readStatistics(statistics, par);
  }
}
