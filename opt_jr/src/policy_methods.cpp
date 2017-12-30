#include "policy_methods.hh"


#include "utility.hh"
#include "objective_fun.hh"
#include "statistics.hh"


#include <string>





///It checks that the total allocated nodes is still less or equal than the total number of cores available N
void Policy_methods::check_total_nodes(int N, Batch &app_manager)
{
  int total = 0;

  for (auto it= app_manager.get_begin(); it!=app_manager.get_end();++it)
  {
    total+= it->get_currentCores_d();
    if (total > N)
    {
      printf("Fatal Error: check_total_nodes: Total current nodes (%d) exceeds maximum nodes number (%d)\n", total, N);
      exit(-1);
    }
  }
}




/**
It estimates the objective function for each move. The pairs of applications for which the move
is profitable are stored in a Candidates object (which is returned).
*/
Candidates Policy_methods::approximated_loop( Batch &app_manager, /*int &iteration,*/ Opt_jr_parameters &par )
{

  std::string debugMsg;


  if (app_manager.get_empty())
  {
    printf("Error: approximatedLoop: NO Applications\n");
    exit(-1);
  }

  int nCoreMov;
  double DELTAVM_i;
  double DELTAVM_j;
  double DELTA_fo_App_i, DELTA_fo_App_j;
  Candidates  app_pairs ;


  debugMsg= "Approximated iterated loop"; par.debug_message(debugMsg);
  auto application_i=app_manager.get_begin();


  while (application_i!=app_manager.get_end())
  {

    auto application_j = app_manager.get_begin();
    while (application_j != app_manager.get_end())
    {
      if (application_i->get_session_app_id()!=application_j->get_session_app_id())
      {
        debugMsg="Comparing " + application_i->get_session_app_id() + " with " + application_j->get_session_app_id();par.debug_message(debugMsg);

        nCoreMov = std::max(application_i->get_V(), application_j->get_V());

        DELTAVM_i = nCoreMov/application_i->get_V(); debugMsg = "app " + application_i->get_session_app_id() + " DELTAVM_i " +  std::to_string(DELTAVM_i); par.debug_message(debugMsg);
        DELTAVM_j = nCoreMov/application_j->get_V(); debugMsg = "app " + application_j->get_session_app_id() + " DELTAVM_j " +  std::to_string(DELTAVM_j); par.debug_message(debugMsg);

        // Change the currentCores, but rollback later
        int deltaNCores_i = DELTAVM_i * application_i->get_V();
        int deltaNCores_j = DELTAVM_j * application_j->get_V();
        application_i->set_currentCores_d( application_i->get_currentCores_d() + deltaNCores_i);
        application_j->set_currentCores_d(application_j->get_currentCores_d() - deltaNCores_j);


        debugMsg= "After cores exchange: app " + application_i->get_session_app_id() + " currentCores " + std::to_string((int)application_i->get_currentCores_d());par.debug_message(debugMsg);
        debugMsg= "After cores exchange: app " + application_j->get_session_app_id() + " currentCores " + std::to_string((int)application_j->get_currentCores_d());par.debug_message(debugMsg);


        if (application_i->get_currentCores_d() > 0 && application_j->get_currentCores_d() > 0)
        {
          DELTA_fo_App_i = Objective_fun::component_approx(*application_i, par) - application_i->get_baseFO();
          debugMsg = "app " + application_i->get_session_app_id() + "DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i);par.debug_message(debugMsg);

          DELTA_fo_App_j = Objective_fun::component_approx(*application_j, par) - application_j->get_baseFO();
          debugMsg = "app " + application_j->get_session_app_id() + "DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j);par.debug_message(debugMsg);


          debugMsg= "\n\n  Approximated TOTAL DELTA FO: " + std::to_string(DELTA_fo_App_i + DELTA_fo_App_j) + "   \n\n "; par.debug_message(debugMsg);


          if ((DELTA_fo_App_i + DELTA_fo_App_j < 0))
          {
            debugMsg= "\n\nAdding candidate  \n\n "; par.debug_message(debugMsg);
            app_pairs.add_candidate( *application_i ,
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


    return app_pairs;

  }









  /**
  It evaluates the delta FO for all the candidates in app_pairs and it does the best change (if it exist) in app_manager
  */
  void Policy_methods::exact_loop(Candidates &app_pairs, Configuration &configuration,  MYSQL *conn,  Batch &app_manager, Opt_jr_parameters &par, int &index_pair)
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


    if(par.get_number_of_threads()==0)
    {
      // Calculate in advance the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
      app_pairs.invoke_predictor_seq( conn, par, configuration);
    }
    else
    {
      // Calculate in advance and in parallel the results of the predictor FOR EACH candidate s.t. currentCores > 0 (until finished or MAX_PROMISING_CONFIGURATIONS is reached)
      app_pairs.invoke_predictor_openMP( par, configuration);
    }

    /*
    *  Find the best pair with deltaFO<0 (if it exists)
    */

    //Those values are useful to detect the best pair
    index_pair=-1;
    index=0;
    DELTA_pair=0;


    for (auto it = app_pairs.get_begin(); it != app_pairs.get_end(); it++)
    {
      if(index>0 && index==MAX_PROMISING_CONFIGURATIONS)//This is done because in invoke_predictor* only the first MAX_PROMISING_CONFIGURATIONS are evaluated; index=0 means par.get_k()==0 i.e. all the pairs are explored
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

        debugMsg = "app " + it->get_session_app_id_i() + " DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i); par.debug_message(debugMsg);
        debugMsg = "app " + it->get_session_app_id_j() + " DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j); par.debug_message(debugMsg);
        debugMsg= "\n      TOTAL DELTA FO : " +  std::to_string(DELTA_tmp) + "\n\n"; par.debug_message(debugMsg);
      }

      index++;

    }


    /*
    *  Find (if any) the corresponding applications in batch and modify them
    */

    if(index_pair!=-1)
    {
      debugMsg="DELTA FO best pair: "+ std::to_string(DELTA_pair);par.debug_message(debugMsg);
      auto it = app_pairs.get_begin();
      for (int j=0; j< index_pair; j++)
      {
        it++;
      }


      for (auto elem= app_manager.get_begin(); elem!=app_manager.get_end(); elem++)
      {
        if (it->get_app_id_i()==elem->get_app_id() && it->get_session_app_id_i()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_new_core_assignment_i());
          elem->set_baseFO( it->get_real_i());
        }
        if (it->get_app_id_j()==elem->get_app_id() && it->get_session_app_id_j()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_new_core_assignment_j());
          elem->set_baseFO( it->get_real_j());
        }
      }
    }
  }
