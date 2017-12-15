#include "search_alterning.hh"
#include "statistics.hh"
#include "objectiveFunction.hh"


void Search_alterning::localSearch(Batch &app_manager, sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)
{
  std::string debugMsg;
  sCandidates sCandidateApproximated ;
  double TotalFO;
  int how_many;
  sStatistics statistics;
  ObjFun OF;

  debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; debugMessage(debugMsg, par);

  //START THE ITERATION LOOP
  for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
  {
    checkTotalNodes(par.get_number(), app_manager);
    debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);


    /*
    *   Estimate the candidates for the predictor
    */
    sCandidateApproximated = approximated_loop( app_manager, how_many, par );
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



    //tmp=app_manager;

    int index_pair;
    exact_loop(sCandidateApproximated, configuration, conn, app_manager, par, index_pair);


    checkTotalNodes(par.get_number(), app_manager);


    if (par.get_globalFOcalculation())
    {
      TotalFO = OF.ObjFunctionGlobal(configuration, conn, app_manager, par);
      std::cout << "\n At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<"\n"<<std::endl;
      addStatistics(statistics, iteration, how_many, TotalFO); // Update Statistics
    }

    if (index_pair==-1)
    {
      break; //It means that even if there are promising configuration they are not convenient
    }

  }



  if (par.get_globalFOcalculation())
  {
    readStatistics(statistics, par);
  }

}
