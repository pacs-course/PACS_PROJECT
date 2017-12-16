#include "search_separing.hh"
#include "statistics.hh"
#include "objectiveFunction.hh"


void Search_separing::localSearch(Batch &app_manager, sConfiguration &configuration, MYSQL *conn,  optJrParameters &par)
{
  std::string debugMsg;
  sCandidates app_pairs ;
  double TotalFO;
  int how_many;
  sStatistics statistics;
  ObjFun OF;

  debugMsg =  "\n     ***** Estimate the candidates for the predictor ******\n"; debugMessage(debugMsg, par);

  //START THE ITERATION LOOP
  for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)//to be changed!
  {
    checkTotalNodes(par.get_number(), app_manager);
    debugMsg= "ITERATION " + std::to_string(iteration); debugMessage(debugMsg, par);


    /*
    *   Estimate the candidates for the predictor
    */
    app_pairs = approximated_loop( app_manager, how_many, par );
    debugMsg= "\n\n\n\n       finished approximatedLoop   \n\n\n\n"; debugMessage(debugMsg, par);

    if (app_pairs.cand.empty())
    {
      // The Candidate Application is empty. No further solution enhancements possible
      debugMsg = "LocalSearch: empty Candidate Application : exit to Approximated loop"; debugMessage(debugMsg, par);
      break;
    }

    debugMsg = " \n\n\n\n\n*****  Ex-iteration loop ******\n\n\n\n\n";debugMessage(debugMsg, par);



    debugMsg = " There are " + std::to_string(how_many) +" promising configurations in iteration " + std::to_string(iteration) + "\n\n"; debugMessage(debugMsg,par);



    //modify the most promising pair in app_manager
    double DELTA_tmp, DELTA_pair=0;
    int index=0, index_pair;

    for (auto it = app_pairs.cand.begin(); it != app_pairs.cand.end(); it++)
    {
      DELTA_tmp=it->delta_i+it->delta_j;

      if (DELTA_tmp<DELTA_pair)
      {
        DELTA_pair=DELTA_tmp;
        index_pair=index;
      }
      index++;
    }



    /*
    *  Find the corresponding applications in batch and modify them
    */
    auto it = app_pairs.cand.begin();
    for (int j=0; j< index_pair; j++)
    {
      it++;
    }


    for (auto &elem : app_manager.APPs)
    {
      if(  it->app_i.get_app_id()==elem.get_app_id())
      {
        if (it->app_i.get_session_app_id()==elem.get_session_app_id())
        {

          std::cout<< "\nehilaa\n";
          elem.set_currentCores_d( it->newCoreAssignment_i);
          elem.set_baseFO( it->real_i);
        }
      }

      if(  it->app_j.get_app_id()==elem.get_app_id())
      {
        if (it->app_j.get_session_app_id()==elem.get_session_app_id())
        {
          elem.set_currentCores_d( it->newCoreAssignment_j);
          elem.set_baseFO( it->real_j);
        }
      }

    }
  }




  app_manager.initialize(configuration, conn, par);

  int indicator=0;
  int nCoreMov;
  double DELTAVM_i;
  double DELTAVM_j;
  double DELTA_fo_App_i=0, DELTA_fo_App_j=0;//Those value are not really used
  for (int iteration = 1; iteration <= par.get_maxIteration(); iteration++)
  {
    sCandidates all_pairs;
    /*
    save ALL the potential exchanges in a sCandidate object
    */

    auto application_i=app_manager.APPs.begin();
    while (application_i!=app_manager.APPs.end())
    {
      auto application_j = app_manager.APPs.begin();
      while (application_j != app_manager.APPs.end())
      {
        if (application_i->get_session_app_id()!=application_j->get_session_app_id())
        {
          nCoreMov = std::max(application_i->get_V(), application_j->get_V());
          DELTAVM_i = nCoreMov/application_i->get_V(); debugMsg = "app " + application_i->get_session_app_id() + " DELTAVM_i " +  std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
          DELTAVM_j = nCoreMov/application_j->get_V(); debugMsg = "app " + application_j->get_session_app_id() + " DELTAVM_j " +  std::to_string(DELTAVM_j); debugMessage(debugMsg, par);

          // Change the currentCores, but rollback later
          int deltaNCores_i = DELTAVM_i * application_i->get_V();
          int deltaNCores_j = DELTAVM_j * application_j->get_V();
          application_i->set_currentCores_d( application_i->get_currentCores_d() + deltaNCores_i);
          application_j->set_currentCores_d(application_j->get_currentCores_d() - deltaNCores_j);

          if (application_i->get_currentCores_d() > 0 && application_j->get_currentCores_d() > 0)
          {
            all_pairs.addCandidate( *application_i ,
              *application_j ,
              application_i->get_currentCores_d(),
              application_j->get_currentCores_d(),
              DELTA_fo_App_i + DELTA_fo_App_j,
              DELTAVM_i,
              DELTAVM_j) ;
            }

            application_i->set_currentCores_d( application_i->get_currentCores_d() - DELTAVM_i*application_i->get_V());
            application_j->set_currentCores_d( application_j->get_currentCores_d() + DELTAVM_j*application_j->get_V());
          }
          application_j++;
        }
        application_i++;
      }

      // exact_loop also updates app_manager
      exact_loop(all_pairs, configuration, conn, app_manager, par, indicator);

      
      checkTotalNodes(par.get_number(), app_manager);
      if (indicator==-1) //It means that there are not convenient exchanges
      {
        debugMsg="No more convenient exchanges."; debugMessage(debugMsg,par);
        break;
      }


      if (par.get_globalFOcalculation())
      {
        TotalFO = OF.ObjFunctionGlobal(configuration, conn, app_manager, par);
        std::cout << "\n At iteration: "<< iteration << " GLOBAL OBJECTIVE FUNCTION = "<< TotalFO <<"\n"<<std::endl;
        addStatistics(statistics, iteration, how_many, TotalFO); // Update Statistics
      }
    }


    if (par.get_globalFOcalculation())
    {
      readStatistics(statistics, par);
    }

  }
