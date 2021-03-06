#include "candidates.hh"

#include "utility.hh"
#include "db.hh"
#include "objective_fun.hh"

#include <omp.h>
#include <iostream>
#include <unordered_map>
#include <utility>



/**
add_candidate build a Candidate_pair object and stores it in a Candidates container
ordered by increasing delta FO
*/
void Candidates::add_candidate(  Application app_i, Application app_j, int contr1, int contr2, double delta, double delta_i, double delta_j)
{

     if (cand.empty())
     {
       cand.emplace(cand.begin(), app_i, app_j, contr1, contr2, delta, delta_i, delta_j);
       return;
     }
     for (auto it= cand.begin(); it!=cand.end(); it++)
     {
       if ( it->get_delta_fo() > delta)
       {
         cand.emplace(it, app_i, app_j, contr1, contr2, delta, delta_i, delta_j );
         return;
       }
     }
     cand.emplace(cand.end(), app_i, app_j , contr1, contr2, delta, delta_i, delta_j);
}



/**
invoke_predictor_openMP calls in parallel the evaluation of the objective function for each pair of applications
and it stores the results in real_i, real_j (members of Candidate_pair).
*/
void Candidates::invoke_predictor_openMP(  Opt_jr_parameters &par, Configuration  &configuration )
{

  std::string debugMsg;
  int n_threads = par.get_number_of_threads();
  int MAX_PROMISING_CONFIGURATIONS =par.get_K();
  MYSQL *conn2[n_threads]; //open a connection for each thread
  std::vector<Application> aux;
  int indicator_i, indicator_j;


  debugMsg= "Executing invoke_predictor_openMP"; par.debug_message(debugMsg);


  if (cand.size()==0) return;

  debugMsg="invokePredictorInAdvance"; par.debug_message(debugMsg);
  for (int i = 0; i< n_threads;++i)
  {
      conn2[i]=DBopen(
                    const_cast<char*>(configuration["OptDB_IP"].c_str()),
                    const_cast<char*>(configuration["DB_port"].c_str()),
                const_cast<char*>(configuration["OptDB_user"].c_str()),
                const_cast<char*>(configuration["OptDB_pass"].c_str()),
                const_cast<char*>(configuration["OptDB_dbName"].c_str())
                );

  }

  /*
  since some pairs could share the same application object, an auxiliary vector is used in order to
  avoid unnecessary calls (and avoid error);
  */




  for (auto it=cand.begin(); it!= cand.end();it++)
  {

    indicator_i =1;
    indicator_j =1;
    for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
    {
      if ( aux_it->get_app_id()== it->get_app_id_i()  &&  aux_it->get_session_app_id()== it->get_session_app_id_i() && aux_it->get_currentCores_d()== it->get_currentCores_d_i() )
      {
        indicator_i=-1;
      }
      if ( aux_it->get_app_id()== it->get_app_id_j()  &&  aux_it->get_session_app_id()== it->get_session_app_id_j() && aux_it->get_currentCores_d()== it->get_currentCores_d_j() )
      {
        indicator_j=-1;
      }

    }
    if (indicator_i==1)
    {
      aux.push_back(it->get_app_i());
    }
    if (indicator_j==1)
    {
      aux.push_back(it->get_app_j());
    }

  }

  debugMsg= "Unique applications in candidates:\n";
  for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
  {
    debugMsg += aux_it->get_session_app_id() + " " + aux_it->get_app_id() + " " + std::to_string(aux_it->get_currentCores_d())+"\n";
  }
  debugMsg+="\n\n"; par.debug_message(debugMsg);

  debugMsg= "Complete candidates list:\n";
  for (auto it=cand.begin(); it!= cand.end();it++)
  {
    debugMsg += it->get_session_app_id_i() + " " + it->get_app_id_i() + " " + std::to_string(it->get_currentCores_d_i()) + "\n";
    debugMsg += it->get_session_app_id_j() + " " + it->get_app_id_j() + " " + std::to_string(it->get_currentCores_d_j()) + "\n\n";
  }
  debugMsg += "\n\n"; par.debug_message(debugMsg);



  #pragma omp parallel num_threads(n_threads)
  {
    int ID=omp_get_thread_num();
    int index=0;
    int j=0;

    for (auto it=aux.begin(); it!=aux.end(); ++it ) // assign each app to a thread
    {
      if (index > 0 && index == 2*MAX_PROMISING_CONFIGURATIONS)
      {
        debugMsg="invoke_predictor_seq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invoke_predictor_seq loop";par.debug_message(debugMsg);
        break;
      }

      int pos=j % n_threads;

      if(pos==ID)
      {
        if (it->get_currentCores_d() > 0 )
        {
          it->set_R_d(Objective_fun::component(configuration, conn2[ID], *it, par));
        }
      }
      ++j;
    }
  }



  for (auto it=cand.begin(); it!= cand.end();it++) // store the results just computed
  {
    for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
    {
      if ( aux_it->get_app_id()== it->get_app_id_i()  &&  aux_it->get_session_app_id()== it->get_session_app_id_i() && aux_it->get_currentCores_d()== it->get_currentCores_d_i() )
      {
        it->set_real_i( aux_it->get_R_d());
      }
      if ( aux_it->get_app_id()== it->get_app_id_j()  &&  aux_it->get_session_app_id()== it->get_session_app_id_j() && aux_it->get_currentCores_d()== it->get_currentCores_d_j() )
      {
        it->set_real_j( aux_it->get_R_d());
      }
    }
  }



  for (int i = 0; i < n_threads;++i)
    DBclose(conn2[i]);

}






void Candidates::invoke_predictor_seq(MYSQL *conn, Opt_jr_parameters &par, Configuration  &configuration )
{
  std::string debugMsg;
  debugMsg= "Executing invoke_predictor_seq"; par.debug_message(debugMsg);

  int MAX_PROMISING_CONFIGURATIONS =par.get_K();
  int index=0;
  std::vector<Application> aux;
  int indicator_i, indicator_j;

  for (auto it=cand.begin(); it!= cand.end();it++)
  {

    indicator_i =1;
    indicator_j =1;
    for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
    {
      if ( aux_it->get_app_id()== it->get_app_id_i()  &&  aux_it->get_session_app_id()== it->get_session_app_id_i() && aux_it->get_currentCores_d()== it->get_currentCores_d_i() )
      {
        indicator_i=-1;
      }
      if ( aux_it->get_app_id()== it->get_app_id_j()  &&  aux_it->get_session_app_id()== it->get_session_app_id_j() && aux_it->get_currentCores_d()== it->get_currentCores_d_j() )
      {
        indicator_j=-1;
      }

    }
    if (indicator_i==1)
    {
      aux.push_back(it->get_app_i());
    }
    if (indicator_j==1)
    {
      aux.push_back(it->get_app_j());
    }

  }


  debugMsg= "Unique applications in candidates:\n";
  for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
  {
    debugMsg += aux_it->get_session_app_id() + " " + aux_it->get_app_id() + " " + std::to_string(aux_it->get_currentCores_d())+"\n";
  }
  debugMsg+="\n\n"; par.debug_message(debugMsg);

  debugMsg= "Complete candidates list:\n";
  for (auto it=cand.begin(); it!= cand.end();it++)
  {
    debugMsg += it->get_session_app_id_i() + " " + it->get_app_id_i() + " " + std::to_string(it->get_currentCores_d_i()) + "\n";
    debugMsg += it->get_session_app_id_j() + " " + it->get_app_id_j() + " " + std::to_string(it->get_currentCores_d_j()) + "\n\n";
  }
  debugMsg += "\n\n"; par.debug_message(debugMsg);



  for (auto it=aux.begin(); it!=aux.end(); ++it )
  {
    if (index > 0 && index == 2*MAX_PROMISING_CONFIGURATIONS)
    {
      debugMsg="invoke_predictor_seq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invoke_predictor_seq loop";par.debug_message(debugMsg);
      break;
    }


    if (it->get_currentCores_d() > 0 )
    {
      it->set_R_d(Objective_fun::component(configuration, conn, *it, par));
    }

  }



  for (auto it=cand.begin(); it!= cand.end();it++) // store the results just computed
  {
    for (auto aux_it= aux.begin();aux_it!= aux.end();aux_it++)
    {
      if ( aux_it->get_app_id()== it->get_app_id_i()  &&  aux_it->get_session_app_id()== it->get_session_app_id_i() && aux_it->get_currentCores_d()== it->get_currentCores_d_i() )
      {
        it->set_real_i( aux_it->get_R_d());
      }
      if ( aux_it->get_app_id()== it->get_app_id_j()  &&  aux_it->get_session_app_id()== it->get_session_app_id_j() && aux_it->get_currentCores_d()== it->get_currentCores_d_j() )
      {
        it->set_real_j( aux_it->get_R_d());
      }
    }
  }
  
}
