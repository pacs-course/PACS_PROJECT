#include "candidates.hh"

#include "utility.hh"
#include "db.hh"
#include "objectiveFunction.hh"

#include <omp.h>



/*
 *       Name:               addCandidate
 *       Description:         This function adds all the information regarding the localSearch deltafo calculation.
 *                            The list is sorted by deltafo value.
 */
void addCandidate(sCandidates  &cand,  Application &app_i, Application &app_j, int contr1, int contr2, double delta, double delta_i, double delta_j)
{

     if (cand.empty())
     {
       cand.emplace(cand.begin(), &app_i, &app_j, contr1, contr2, delta, delta_i, delta_j);
       return;
     }
     for (auto it= cand.begin(); it!=cand.end(); it++)
     {
       if ( it->deltaFO < delta)
       {
         cand.emplace(it, &app_i, &app_j, contr1, contr2, delta, delta_i, delta_j );
         return;
       }
     }
     cand.emplace(cand.end(), &app_i, &app_j , contr1, contr2, delta, delta_i, delta_j);
}



void invokePredictorOpenMP(sCandidates  &candidates,  optJrParameters &par, sConfiguration  &configuration )
{
  std::string debugMsg;


  int n_threads = par.get_numberOfThreads();

  MYSQL *conn2[n_threads]; //open a connection for each thread


  if (candidates.size()==0) return;

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
    int j=0;


    // assign each app to a thread
    for (auto it=candidates.begin(); it!=candidates.end(); ++it ) // assign each app to a thread
    {

      int pos=j % n_threads;

      if(pos==ID)
      {

        it->real_i = ObjFun::ObjFunctionComponent(configuration, conn2[ID], *(it->app_i), par);
        //it->nodes_i = it->app_i->currentCores_d;

        it->real_j = ObjFun::ObjFunctionComponent(configuration, conn2[ID], *(it->app_j), par);
        //it->nodes_j = it->app_j->currentCores_d;

      }
      ++j;
    }

  }
  for (int i = 0; i < n_threads;++i)
    DBclose(conn2[i]);

}
