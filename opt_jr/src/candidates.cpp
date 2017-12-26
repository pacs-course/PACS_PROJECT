#include "candidates.hh"

#include "utility.hh"
#include "db.hh"
#include "objective_fun.hh"

#include <omp.h>



/*
 *       Name:               add_candidate
 *       Description:         This function adds all the information regarding the local_search deltafo calculation in a candidate object and it stores the object in the list.
 *                            The list is sorted by deltafo value.
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



void Candidates::invoke_predictor_openMP(  Opt_jr_parameters &par, Configuration  &configuration )
{

  std::string debugMsg;
  int n_threads = par.get_number_of_threads();
  int MAX_PROMISING_CONFIGURATIONS =par.get_K();
  MYSQL *conn2[n_threads]; //open a connection for each thread

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
  since some pairs could share the same application object, I use an auxiliary vector in order to
  avoid unnecessary calls (and avoid error);
  */

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

  //call invokePredictorInAdvance in parallel
  #pragma omp parallel num_threads(n_threads)
  {
    int ID=omp_get_thread_num();
    int index=0;
    int j=0;
    Objective_fun OF;
    //double tmp;

    // assign each app to a thread

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
        if (it->get_currentCores_d() > 0 )//it->app_i.get_currentCores_d() > 0 && it->app_j.get_currentCores_d() > 0)
        {
          OF.component(configuration, conn2[ID], *it, par); //caches the results
          //it->nodes_i = it->app_i->get_currentCores_d();

          //it->real_j = OF.component(configuration, conn2[ID], (it->app_j), par);
          //it->nodes_j = it->app_j->get_currentCores_d();
        }

      }
      ++j;
    }

  }


  #pragma omp parallel num_threads(n_threads)
  {
    int ID=omp_get_thread_num();
    int index=0;
    int j=0;
    Objective_fun OF;

    // assign each app-pairs to a thread
    for (auto it=cand.begin(); it!=cand.end(); ++it )
    {
      if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
      {
        debugMsg="invoke_predictor_seq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invoke_predictor_seq loop";par.debug_message(debugMsg);
        break;
      }

      int pos=j % n_threads;

      if(pos==ID)
      {
        if (it->get_currentCores_d_i() > 0 && it->get_currentCores_d_j() > 0)
        {

          it->set_real_i( OF.component(configuration, conn2[ID], ( it->get_app_i_ref() ), par)); //Already cached
          //it->nodes_i = it->app_i->get_currentCores_d();

          it->set_real_j( OF.component(configuration, conn2[ID], (it->get_app_j_ref()), par)); //Already cached
          //it->nodes_j = it->app_j->get_currentCores_d();
        }

      }
      ++j;
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
  Objective_fun OF;

  for (auto it = cand.begin(); it != cand.end(); it++)
  {
    // Consider only the first MAX_PROMISING_CONFIGURATIONS (0 value means browse the entire Application) Application members
    if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
    {
      debugMsg="invoke_predictor_seq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invoke_predictor_seq loop";par.debug_message(debugMsg);
      break;
    }



    if (it->get_currentCores_d_i() > 0 && it->get_currentCores_d_j() > 0)
    {
      debugMsg= "Comparing " + it->get_session_app_id_i() + " with " + it->get_session_app_id_j(); par.debug_message(debugMsg);
      //it->app_i.mode= R_ALGORITHM; it->app_j.mode= R_ALGORITHM;
      // No openmp
      debugMsg =  " CALLING OBJ_FUNCTION_COMPONENT \n\n"; par.debug_message(debugMsg);
      it->set_real_i ( OF.component(configuration, conn, it->get_app_i_ref(), par));
      it->set_real_j ( OF.component(configuration, conn, it->get_app_j_ref(), par));
    }
    index++;
  }
}
