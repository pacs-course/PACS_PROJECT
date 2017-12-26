#include "batch.hh"




#include "objective_fun.hh"
#include "db.hh"

#include <iostream>
#include <string>
#include <cmath>
#include <list>





/*
 * 		Name:					calculate_nu
 * 		Description: - it computes the nu indice for the first application
 * 								 - it computes the nu indices for all the other applications
 */

void
Batch::calculate_nu(Opt_jr_parameters &par)
{

  std::string debugMsg;
  double w1;
  double chi_c_1;
  double csi_1;
  int N = par.get_number(); // N is the number of cores
  double csi;

  int minCapacity= 0;

  debugMsg="Calculate nu indices and bounds for each application"; par.debug_message(debugMsg);
  if (APPs.begin()==APPs.end())
  {
    printf("Fatal error: There are not Applications\n");
    exit(-1);
  }

  /*
   * COMPUTE THE MINIMUM CAPACITY TO EXECUTE THE APPLICATIONS
   */
  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    minCapacity+= it->get_V();
  }

  if(N<minCapacity)
  {
    std::cout << "<fatal error>: The number of cores is lower than the minimum capacity requirement "<<std::endl;
    std::cout << "minimim capacity: "<<minCapacity<<std::endl;
    std::cout << "number of cores: "<<N<<std::endl;
    exit(-1);
  }
  N = N - minCapacity; // N now is the number of cores available to exchange
  debugMsg="available cores to exchange ="+ std::to_string(N); par.debug_message(debugMsg);


  /*
   *  COMPUTE NU_1
   */
  double tot = 0;
  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it==APPs.begin()) // First app only
    {
      w1 = it->get_w();
      chi_c_1 = it->get_chi_C();
      csi_1 = std::max(it->get_M()/it->get_m(), it->get_V()/it->get_v());
    }
    else //Any other row (compute tot)
    {
      csi = std::max(it->get_M()/it->get_m(), it->get_V()/it->get_v());
      it->set_term_i(  sqrt( (it->get_w()/w1)*(it->get_chi_C()/chi_c_1)*(csi_1/csi)));
      tot = tot + it->get_term_i();
    }
  }

  double nu_1 = N/(1 + tot);

  /*
   * COMPUTE NU_i (!=NU_1) AND ASSIGN VALUES
   */

  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it==APPs.begin()) it->set_nu_d(nu_1);
    else
    {
      it->set_nu_d( (it->get_term_i()/(1 + tot))*N);
    }

    it->set_currentCores_d( it->get_nu_d());
    debugMsg="App ID: " + it->get_app_id() + ", NU: " + std::to_string(it->get_nu_d()); par.debug_message(debugMsg);




  }
  debugMsg="end calculate nu"; par.debug_message(debugMsg);


};


/*
 * Name:				initialize
 * Description			For each application, a base value for the objective function is calculated.
 */

void Batch::initialize(Configuration  &configuration, MYSQL *conn, Opt_jr_parameters &par)
{
	std::string debugMsg;

  debugMsg =  "Information: INITIALIZE baseFo for all the applications" ;par.debug_message(debugMsg);
	for (auto it =APPs.begin(); it!= APPs.end(); it++)
	{

			it->set_baseFO( Objective_fun::component(configuration, conn, *it, par));
			debugMsg = "INITIALIZE BASE FO for APP "+ it->get_session_app_id()
                + " baseFO = " + std::to_string(it->get_baseFO())+"\n"; par.debug_message(debugMsg);
	}


};



/*
 * Name: fix_initial_solution
 * Description: It fixes the initial solution by reallocating the residual
 *              cores to the applications that may need more resources
 */

void Batch::fix_initial_solution(Opt_jr_parameters &par)
{

	int allocatedCores;
  std::list< Application* > LP; // Auxiliary list, to store "suffering" app pointers in weight order

	int loopExit = 0;
	int residualCores;
  std::string debugMsg;
	int N = par.get_number();

	allocatedCores = 0;

	for(auto it = APPs.begin(); it!=APPs.end(); ++it)
	{

		it->set_currentCores_d( std::max(((int)(it->get_currentCores_d() / it->get_V())) * it->get_V(),it->get_V()));
		if (it->get_currentCores_d() > it->get_bound())
			it->set_currentCores_d(  it->get_bound() );
		else
			{
        //add the application to the list of applications pointer ordered  by weight
				debugMsg= "adding " + it->get_session_app_id() + " to ApplicationPointers"; par.debug_message(debugMsg);
        if (LP.empty())
        {
          LP.emplace(LP.begin(), &(*it));
        }
        else
        {
          auto it_l= LP.begin();
          while (it_l!=LP.end())
          {
            if ( (*it_l)->get_w() < it->get_w())
            {
              LP.emplace(it_l, &(*it));
              break;
            }
            it_l++;
          }
          if(it_l==LP.end())
          LP.emplace(LP.end(), &(*it));
        }
			}

		allocatedCores+= it->get_currentCores_d();
		debugMsg =  "fix_initial_solution FIXING CORES "+  it->get_session_app_id()
                + " cores: " + std::to_string(it->get_currentCores_d()); par.debug_message(debugMsg);
	}

	debugMsg= "fix_initial_solution: allocatedCores "+ std::to_string(allocatedCores); par.debug_message(debugMsg);



	residualCores = N - allocatedCores;

  if (residualCores<0) //NOTE: if true this is a very strange situations; if it happens an idea could be to change
  {                    //      source file in such a way that cores are taken from applications with less weight
    std::cout<< "FATAL ERROR: limit situation, too few cores. Allocated cores exceed the number of available cores \n";
    exit(-1);
  }

  debugMsg = "RESIDUAL CORES: "+ std::to_string(residualCores); par.debug_message(debugMsg);
	int addedCores;


  //I browse the list of "suffering" apps in order of weight, as long as I have available cores


  auto it=LP.begin();
	while (!loopExit&& (residualCores>0))
	{
		if (it==LP.end() ) loopExit = 1;
		else
		{

			int potentialDeltaCores=((int)(residualCores / (*it)->get_V() ) )* (*it)->get_V();

			if (((*it)->get_currentCores_d() + potentialDeltaCores) > (*it)->get_bound()){
				addedCores = (*it)->get_bound() - (*it)->get_currentCores_d() ;
			  (*it)->set_currentCores_d( (*it)->get_bound());

			}
			else{
				(*it)->set_currentCores_d( (*it)->get_currentCores_d() + potentialDeltaCores);
				addedCores=potentialDeltaCores;
			}

			if ((*it)->get_currentCores_d() == 0)
			{
				printf("\nFatal Error: FixInitialSolution: app %s has %d cores after fix\n", (*it)->get_session_app_id().c_str(), (*it)->get_currentCores_d());
				exit(-1);
			}
			if (addedCores > 0)
			{

				debugMsg="adding cores to App " + (*it)->get_session_app_id() + " added Cores: " +  std::to_string(addedCores) ;par.debug_message(debugMsg);
				debugMsg=" application_id " + (*it)->get_session_app_id() + " new cores " + std::to_string((int)(*it)->get_currentCores_d()) + " moved cores "+ std::to_string(addedCores) ;par.debug_message(debugMsg);
				residualCores = residualCores - addedCores;
			}
			it++;
		}

		if (residualCores == 0) loopExit = 1;
	}


}

/*
 * 		Name:					write_results
 * 		Description:			This function prints the results of the localSearch application (number of cores and VM) in a DB table.
 * 								If a result for a (session_id, application_id) already exists, then it is replaced.
 *
 */


void Batch::write_results(MYSQL *conn, char * dbName, Opt_jr_parameters &par)
{
	std::string debugMsg;

	char sqlStatement[512];

	debugMsg= "write_results"; par.debug_message(debugMsg);
	if (APPs.empty())
	{
		printf("FATAL ERROR: write_results: APPs cannot be empty\n");
		exit(-1);
	}
	//while (pointer!=NULL)
	for(auto &elem : APPs)
	{
		"Session ID " + elem.get_session_app_id()+ " Application Id " + elem.get_app_id()  + " cores " + std::to_string(elem.get_currentCores_d())+ " VMs "+ std::to_string(elem.get_vm()); par.debug_message(debugMsg);

		// Check if the result of the computation for that session, application has been already computed and stored previously
		sprintf(sqlStatement, "select opt_id, app_id from %s.OPT_SESSIONS_RESULTS_TABLE where opt_id='%s' and app_id='%s'",
				dbName,  par.get_filename().c_str(),
				elem.get_session_app_id().c_str());
		MYSQL_ROW row = executeSQL(conn, sqlStatement, par);

		if (row == NULL)
		{
			sprintf(sqlStatement, "insert %s.OPT_SESSIONS_RESULTS_TABLE values('%s', '%s',%d, %d)",
								dbName,
								par.get_filename().c_str(),
								elem.get_session_app_id().c_str(),
								elem.get_currentCores_d(),
								elem.get_vm()
						);
			if (mysql_query(conn, sqlStatement))
			{
				char error[512];
				sprintf(error, " %s", sqlStatement);
				DBerror(conn, error);
			}
		}
		else // Perform an update
		{
			sprintf(sqlStatement, "update %s.OPT_SESSIONS_RESULTS_TABLE set opt_id = '%s', app_id = '%s',num_cores = %d, num_vm = %d where opt_id='%s' and app_id='%s'",
											dbName,
											par.get_filename().c_str(),
											elem.get_session_app_id().c_str(),
											elem.get_currentCores_d(),
											elem.get_vm(),
											par.get_filename().c_str(),
											elem.get_session_app_id().c_str()
									);
			if (mysql_query(conn, sqlStatement))
						{

							char error[512];
							sprintf(error, " %s", sqlStatement);
							DBerror(conn, error);
						}
		}

	}

}





std::string Batch::show_session_app_id()
{
  std::string Msg;
  Msg= "session_app_id of loaded applications:  \n";
  for (auto it= APPs.begin(); it!=APPs.end();++it)
    Msg+= "App_ID: "+ it->get_session_app_id() +"\n";
  return Msg;
}


std::string Batch::show_bounds()
{
  std::string Msg;
  for (auto it= APPs.begin(); it!= APPs.end();it++)
  {
    Msg+="\nBound evaluated for Session_app_id : " + it->get_session_app_id() + " , APP_ID: " + it->get_app_id() +
             ", Deadline = "+ std::to_string(it->get_Deadline_d()) + ", R =" + std::to_string(it->get_R_d())+
             ", bound = "+ std::to_string(it->get_bound()) ;
  }
  return Msg;
}



std::string Batch::show_solution()
{
  std::string Msg;
  for (auto it = APPs.begin(); it!=APPs.end(); ++it)
  {

    Msg+= "\n Application   " + it->get_session_app_id() + "      w = " + std::to_string(it->get_w())
             + "     ncores = "  + std::to_string( it->get_currentCores_d()) +  "      FO = " + std::to_string( it->get_baseFO() );
  }
  return Msg;
}
