#include "bounds.hh"
#include "debugmessage.hh"
#include "db.hh"
#include <omp.h>
#include <string>



void Bounds::calculateBounds(Batch  & app_manager, int n_threads,
                             sConfiguration &configuration, MYSQL *conn,
                             optJrParameters &par)
  {
    std::string debugMsg;
    debugMsg=" Calculate bounds for each application ";debugMessage(debugMsg,par);

    //sList* t_pointer[n_threads];
    MYSQL *conn2[n_threads];
    for (int i =0; i< n_threads;++i)
    {
      //t_pointer[i]=pointer;
      conn2[i]=DBopen(
        const_cast<char*>(configuration["OptDB_IP"].c_str()),
        const_cast<char*>(configuration["OptDB_user"].c_str()),
        const_cast<char*>(configuration["OptDB_pass"].c_str()),
        const_cast<char*>(configuration["OptDB_dbName"].c_str())
      );
    }
    /*call findbound in parallel;*/
    #pragma omp parallel num_threads(n_threads)
    {
      int ID=omp_get_thread_num();
      int j=0;
      /* assign each app to a thread */
      for (auto it=app_manager.APPs.begin(); it !=app_manager.APPs.end(); it++)
      {
        int pos=j%n_threads;
        debugMsg= "pos= " + std::to_string(pos)
                 +", j= " + std::to_string(j)
                 + ", ID= " + std::to_string(ID);debugMessage(debugMsg,par);

        if(pos==ID)
        {
          debugMsg= "findBound of app number " + std::to_string(j)
                  + "called from thread" + std::to_string(ID);debugMessage(debugMsg,par);

          //findBound(configuration, conn2[ID], getConfigurationValue(configuration, "OptDB_dbName"), t_pointer[ID], par);
        }
        ++j;
      }
    }
    debugMsg= "****************** End calculate bounds ***************** ";debugMessage(debugMsg,par);


  }
