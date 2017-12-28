#include "policy_separing.hh"
#include "statistics.hh"
#include "objective_fun.hh"

/**
The local_search methods performs first an approximated loop in which the most profitable
move is done at each iteration, then it performs an exact loop in which the objective
function is evaluated for all possible pairs exchanges at each iteration.
*/
void Policy_separing::local_search(Batch &app_manager, Configuration &configuration, MYSQL *conn,  Opt_jr_parameters &par)
{
  std::string debugMsg;
  Candidates app_pairs ;
  double TotalFO;
  int how_many;
  Statistics statistics;

  debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; par.debug_message(debugMsg);

  //START THE ITERATION LOOP
  for (int iteration = 1; iteration <= N_APPROX_LOOP*par.get_max_iterations(); iteration++)//to be changed!
  {
    std::cout<<"\n\n\n"<<iteration;
    check_total_nodes(par.get_number(), app_manager);
    debugMsg= "ITERATION " + std::to_string(iteration); par.debug_message(debugMsg);


    /*
    *   Estimate the candidates for the predictor
    */
    app_pairs = approximated_loop( app_manager, par );
    debugMsg= "\n\n\n\n       finished approximatedLoop   \n\n\n\n"; par.debug_message(debugMsg);

    if (app_pairs.get_empty())
    {
      // The Candidate Application is empty. No further solution enhancements possible
      debugMsg = "LocalSearch: empty Candidate Application : exit to Approximated loop"; par.debug_message(debugMsg);
      break;
    }

    debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";par.debug_message(debugMsg);


    how_many=app_pairs.get_size();
    debugMsg = " There are " + std::to_string(how_many) +" promising configurations in iteration " + std::to_string(iteration) + "\n\n"; par.debug_message(debugMsg);





    /*
    *  Find the corresponding application in batch and modify it
    */


    auto it = app_pairs.get_begin(); // the list is ordered so the best pair is the first (already checked that the list is not empty)

    for (auto elem= app_manager.get_begin(); elem!=app_manager.get_end(); elem++)
    {
      if(  it->get_app_id_i()==elem->get_app_id())
      {
        if (it->get_session_app_id_i()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_new_core_assignment_i());
          elem->set_baseFO( it->get_real_i());
        }
      }

      if(  it->get_app_id_j()==elem->get_app_id())
      {
        if (it->get_session_app_id_j()==elem->get_session_app_id())
        {
          elem->set_currentCores_d( it->get_new_core_assignment_j());
          elem->set_baseFO( it->get_real_j());
        }
      }


    }

  }




  app_manager.initialize(configuration, conn, par); //evaluate a base value for FO

  int indicator=0;
  int nCoreMov;
  double DELTAVM_i;
  double DELTAVM_j;
  double DELTA_fo_App_i=0, DELTA_fo_App_j=0;//Those value are not really used
  for (int iteration = 1; iteration <= par.get_max_iterations(); iteration++)
  {
    Candidates all_pairs;
    /*
    save ALL the potential exchanges in a Candidates object
    */

    auto application_i=app_manager.get_begin();
    while (application_i!=app_manager.get_end())
    {
      auto application_j = app_manager.get_begin();
      while (application_j != app_manager.get_end())
      {
        if (application_i->get_session_app_id()!=application_j->get_session_app_id())
        {
          nCoreMov = std::max(application_i->get_V(), application_j->get_V());
          DELTAVM_i = nCoreMov/application_i->get_V(); debugMsg = "app " + application_i->get_session_app_id() + " DELTAVM_i " +  std::to_string(DELTAVM_i); par.debug_message(debugMsg);
          DELTAVM_j = nCoreMov/application_j->get_V(); debugMsg = "app " + application_j->get_session_app_id() + " DELTAVM_j " +  std::to_string(DELTAVM_j); par.debug_message(debugMsg);

          // Change the currentCores, but rollback later
          int deltaNCores_i = DELTAVM_i * application_i->get_V();
          int deltaNCores_j = DELTAVM_j * application_j->get_V();
          application_i->set_currentCores_d( application_i->get_currentCores_d() + deltaNCores_i);
          application_j->set_currentCores_d(application_j->get_currentCores_d() - deltaNCores_j);

          if (application_i->get_currentCores_d() > 0 && application_j->get_currentCores_d() > 0)
          {
            all_pairs.add_candidate( *application_i ,
              *application_j ,
              application_i->get_currentCores_d(),
              application_j->get_currentCores_d(),
              DELTA_fo_App_i + DELTA_fo_App_j,
              DELTAVM_i,
              DELTAVM_j) ;
            }

            //rollback
            application_i->set_currentCores_d( application_i->get_currentCores_d() - DELTAVM_i*application_i->get_V());
            application_j->set_currentCores_d( application_j->get_currentCores_d() + DELTAVM_j*application_j->get_V());
          }
          application_j++;
        }
        application_i++;
      }

      // exact_loop also updates app_manager
      exact_loop(all_pairs, configuration, conn, app_manager, par, indicator);

      check_total_nodes(par.get_number(), app_manager);
      if (indicator==-1) //It means that there are not convenient exchanges
      {
        debugMsg="No more convenient exchanges."; par.debug_message(debugMsg);
        break;
      }


      if (par.get_global_FO_calculation())
      {
        TotalFO = Objective_fun::global(configuration, conn, app_manager, par);
        std::cout << "At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<std::endl;
        statistics.add_statistics( iteration, how_many, TotalFO); // Update Statistics
      }
    }


    if (par.get_global_FO_calculation())
    {
      statistics.read_statistics( par);
    }

  }
