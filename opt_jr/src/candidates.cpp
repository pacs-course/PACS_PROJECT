#include "candidates.hh"

#include "utility.hh"
#include "db.hh"
#include "objectiveFunction.hh"

#include <omp.h>



/*
 *       Name:               addCandidate
 *       Description:         This function adds all the information regarding the localSearch deltafo calculation in a candidate object and it stores the object in the list.
 *                            The list is sorted by deltafo value.
 */
void sCandidates::addCandidate(  Application app_i, Application app_j, int contr1, int contr2, double delta, double delta_i, double delta_j)
{

     if (cand.empty())
     {
       cand.emplace(cand.begin(), app_i, app_j, contr1, contr2, delta, delta_i, delta_j);
       return;
     }
     for (auto it= cand.begin(); it!=cand.end(); it++)
     {
       if ( it->deltaFO > delta)
       {
         cand.emplace(it, app_i, app_j, contr1, contr2, delta, delta_i, delta_j );
         return;
       }
     }
     cand.emplace(cand.end(), app_i, app_j , contr1, contr2, delta, delta_i, delta_j);
}



void sCandidates::invokePredictorOpenMP(  optJrParameters &par, sConfiguration  &configuration )
{

  std::string debugMsg;
  int n_threads = par.get_numberOfThreads();
  int MAX_PROMISING_CONFIGURATIONS =par.get_K();
  MYSQL *conn2[n_threads]; //open a connection for each thread

  debugMsg= "Executing invokePredictorOpenMP"; debugMessage(debugMsg, par);


  if (cand.size()==0) return;

  debugMsg="invokePredictorInAdvance"; debugMessage(debugMsg, par);
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
  //call invokePredictorInAdvance in parallel
  #pragma omp parallel num_threads(n_threads)
  {
    int ID=omp_get_thread_num();
    int index=0;
    int j=0;

    // assign each app to a thread
    for (auto it=cand.begin(); it!=cand.end(); ++it ) // assign each app to a thread
    {
      if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
      {
        debugMsg="invokePredictorSeq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invokePredictorSeq loop";debugMessage(debugMsg, par);
        break;
      }

      int pos=j % n_threads;

      if(pos==ID)
      {
        if (it->app_i.currentCores_d > 0 && it->app_j.currentCores_d > 0)
        {

          it->real_i = ObjFun::ObjFunctionComponent(configuration, conn2[ID], (it->app_i), par);
          //it->nodes_i = it->app_i->currentCores_d;

          it->real_j = ObjFun::ObjFunctionComponent(configuration, conn2[ID], (it->app_j), par);
          //it->nodes_j = it->app_j->currentCores_d;
        }

      }
      ++j;
    }

  }
  for (int i = 0; i < n_threads;++i)
    DBclose(conn2[i]);

}


void sCandidates::invokePredictorSeq(MYSQL *conn, optJrParameters &par, sConfiguration  &configuration )
{
  std::string debugMsg;
  debugMsg= "Executing invokePredictorSeq"; debugMessage(debugMsg, par);

  int MAX_PROMISING_CONFIGURATIONS =par.get_K();
  int index=0;

  for (auto it = cand.begin(); it != cand.end(); it++)
  {
    // Consider only the first MAX_PROMISING_CONFIGURATIONS (0 value means browse the entire Application) Application members
    if (index > 0 && index == MAX_PROMISING_CONFIGURATIONS)
    {
      debugMsg="invokePredictorSeq: MAX_PROMISING_CONFIGURATIONS was reached, leaving invokePredictorSeq loop";debugMessage(debugMsg, par);
      break;
    }



    if (it->app_i.currentCores_d > 0 && it->app_j.currentCores_d > 0)
    {
      debugMsg= "Comparing " + it->app_i.get_session_app_id() + " with " + it->app_j.get_session_app_id(); debugMessage(debugMsg, par);
      it->app_i.mode= R_ALGORITHM; it->app_j.mode= R_ALGORITHM;
      // No openmp
      debugMsg =  " CALLING OBJ_FUNCTION_COMPONENT \n\n"; debugMessage(debugMsg, par);
      it->real_i = ObjFun::ObjFunctionComponent(configuration, conn, it->app_i, par);
      it->real_j = ObjFun::ObjFunctionComponent(configuration, conn, it->app_j, par);
    }
    index++;
  }
}
