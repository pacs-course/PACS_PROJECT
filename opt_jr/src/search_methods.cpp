#include "Search_methods.hh"

#include "debugmessage.hh"
#include "utility.hh"
#include "objectiveFunction.hh"
#include "statistics.hh"


#include <string>






/*
* 		Name:					checkTotalNodes
* 		Description:			It checks that the total allocated nodes is still less or equal than the total number of cores available N
*/

void Search_methods::checkTotalNodes(int N, Batch &App_manager)
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
* Name: approximatedLoop
* Description: It estimates the objective function for each move. The candidate applications for which the move is profitable are stored in a sCandidate object
*/

sCandidates Search_methods::approximated_loop( Batch &App_manager, int &iteration, optJrParameters &par )
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
  sCandidates  app_pairs ;


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
            app_pairs.addCandidate( *application_i ,
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



    iteration= app_pairs.cand.size(); //da modificare

    return app_pairs;

  }









  /*
  * 		Name:					exact_loop
  * 		Description:  It evaluates the delta FO for all the candidates in app_pairs and it does the best change (if it exist) in App_manager
  *
  */

  void Search_methods::exact_loop(sCandidates &app_pairs, sConfiguration &configuration,  MYSQL *conn,  Batch &App_manager, optJrParameters &par, int &index_pair)
  {

    std::string debugMsg;
    int MAX_PROMISING_CONFIGURATIONS = par.get_K();
    double DELTA_fo_App_i, DELTA_fo_App_j;
    int index=0; //Those values are useful to detect the best pair
    double DELTA_pair=0, DELTA_tmp;
    index_pair=-1;


    /*
    *   Evaluate objective function for most promising pairs (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
    */


    if(par.get_numberOfThreads()==0)
    {
      std::cout<<"\n\n invoking sequencial :D\n\n";
      // Calculate in advance the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
      app_pairs.invokePredictorSeq( conn, par, configuration);
    }
    else
    {
      std::cout<<"\n\n invoking parallel :D\n\n";
      // Calculate in advance and in parallel the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
      app_pairs.invokePredictorOpenMP( par, configuration);
    }

    /*
    *  Find the best pair with deltaFO<0 (if it exists)
    */

    //Those values are useful to detect the best pair
    index_pair=-1;
    index=0;
    DELTA_pair=0;


    for (auto it = app_pairs.cand.begin(); it != app_pairs.cand.end(); it++)
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
      debugMsg="DELTA FO best pair: "+ std::to_string(DELTA_pair);debugMessage(debugMsg,par);
      auto it = app_pairs.cand.begin();
      for (int j=0; j< index_pair; j++)
      {
        it++;
      }


      for (auto elem= App_manager.APPs.begin(); elem!=App_manager.APPs.end(); elem++)
      {
        if (it->app_i.get_app_id()==elem->get_app_id() && it->app_i.get_session_app_id()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->newCoreAssignment_i);
          elem->set_baseFO( it->real_i);
        }
        if (it->app_j.get_app_id()==elem->get_app_id() && it->app_j.get_session_app_id()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->newCoreAssignment_j);
          elem->set_baseFO( it->real_j);
        }
      }
    }
  }
