#include "Search_methods.hh"


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

  for (auto it= App_manager.get_begin(); it!=App_manager.get_end();++it)
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

sCandidates Search_methods::approximated_loop( Batch &App_manager, /*int &iteration,*/ optJrParameters &par )
{

  std::string debugMsg;
  ObjFun OF;


  if (App_manager.get_empty())
  {
    printf("Error: approximatedLoop: NO Applications\n");
    exit(-1);
  }

  int nCoreMov;
  double DELTAVM_i;
  double DELTAVM_j;
  double DELTA_fo_App_i, DELTA_fo_App_j;
  sCandidates  app_pairs ;


  debugMsg= "Approximated iterated loop"; par.debugMessage(debugMsg);
  auto application_i=App_manager.get_begin();


  while (application_i!=App_manager.get_end())
  {

    auto application_j = App_manager.get_begin();
    while (application_j != App_manager.get_end())
    {
      if (application_i->get_session_app_id()!=application_j->get_session_app_id())
      {
        debugMsg="Comparing " + application_i->get_session_app_id() + " with " + application_j->get_session_app_id();par.debugMessage(debugMsg);

        nCoreMov = std::max(application_i->get_V(), application_j->get_V());

        DELTAVM_i = nCoreMov/application_i->get_V(); debugMsg = "app " + application_i->get_session_app_id() + " DELTAVM_i " +  std::to_string(DELTAVM_i); par.debugMessage(debugMsg);
        DELTAVM_j = nCoreMov/application_j->get_V(); debugMsg = "app " + application_j->get_session_app_id() + " DELTAVM_j " +  std::to_string(DELTAVM_j); par.debugMessage(debugMsg);

        // Change the currentCores, but rollback later
        int deltaNCores_i = DELTAVM_i * application_i->get_V();
        int deltaNCores_j = DELTAVM_j * application_j->get_V();
        application_i->set_currentCores_d( application_i->get_currentCores_d() + deltaNCores_i);
        application_j->set_currentCores_d(application_j->get_currentCores_d() - deltaNCores_j);


        debugMsg= "After cores exchange: app " + application_i->get_session_app_id() + " currentCores " + std::to_string((int)application_i->get_currentCores_d());par.debugMessage(debugMsg);
        debugMsg= "After cores exchange: app " + application_j->get_session_app_id() + " currentCores " + std::to_string((int)application_j->get_currentCores_d());par.debugMessage(debugMsg);


        if (application_i->get_currentCores_d() > 0 && application_j->get_currentCores_d() > 0)
        {
          DELTA_fo_App_i = OF.ObjFunctionComponentApprox(*application_i, par) - application_i->get_baseFO();
          debugMsg = "app " + application_i->get_session_app_id() + "DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i);par.debugMessage(debugMsg);

          DELTA_fo_App_j = OF.ObjFunctionComponentApprox(*application_j, par) - application_j->get_baseFO();
          debugMsg = "app " + application_j->get_session_app_id() + "DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j);par.debugMessage(debugMsg);


          debugMsg= "\n\n  Approximated TOTAL DELTA FO: " + std::to_string(DELTA_fo_App_i + DELTA_fo_App_j) + "   \n\n "; par.debugMessage(debugMsg);


          if ((DELTA_fo_App_i + DELTA_fo_App_j < 0))
          {
            debugMsg= "\n\nAdding candidate  \n\n "; par.debugMessage(debugMsg);
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



  //  iteration= app_pairs.get_size();

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
      // Calculate in advance the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
      app_pairs.invokePredictorSeq( conn, par, configuration);
    }
    else
    {
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


    //NB: qui la lista dovrebbe essere già ordinata, giusto???? controlla.. dovrebbe servire solo begin e prima verifica su empty

    for (auto it = app_pairs.get_begin(); it != app_pairs.get_end(); it++)
    {
      if(index>0 && index==MAX_PROMISING_CONFIGURATIONS)//This is done because in invokePredictor* only the first MAX_PROMISING_CONFIGURATIONS are evaluated; index=0 means par.get_k()==0 i.e. all the pairs are explored
      {
        break;
      }

      if (it->get_currentCores_d_i() > 0 && it->get_currentCores_d_j() > 0) // For those pairs, object function evaluation calculated earlier
      {

        DELTA_fo_App_i = it->get_real_i() - it->get_base_fo_i();
        DELTA_fo_App_j = it->get_real_j() - it->get_base_fo_j();

        DELTA_tmp=DELTA_fo_App_i+DELTA_fo_App_j;

        if (DELTA_tmp<DELTA_pair)
        {
          DELTA_pair=DELTA_tmp;
          index_pair=index;
        }

        debugMsg = "app " + it->get_session_app_id_i() + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); par.debugMessage(debugMsg);
        debugMsg = "app " + it->get_session_app_id_j() + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); par.debugMessage(debugMsg);
        debugMsg= "\n      TOTAL DELTA FO : " +  std::to_string(DELTA_tmp) + "\n\n"; par.debugMessage(debugMsg);
      }

      index++;

    }


    /*
    *  Find (if any) the corresponding applications in batch and modify them
    */

    if(index_pair!=-1)
    {
      debugMsg="DELTA FO best pair: "+ std::to_string(DELTA_pair);par.debugMessage(debugMsg);
      auto it = app_pairs.get_begin();
      for (int j=0; j< index_pair; j++)
      {
        it++;
      }


      for (auto elem= App_manager.get_begin(); elem!=App_manager.get_end(); elem++)
      {
        if (it->get_app_id_i()==elem->get_app_id() && it->get_session_app_id_i()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_newCoreAssignment_i());
          elem->set_baseFO( it->get_real_i());
        }
        if (it->get_app_id_j()==elem->get_app_id() && it->get_session_app_id_j()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_newCoreAssignment_j());
          elem->set_baseFO( it->get_real_j());
        }
      }
    }
  }
