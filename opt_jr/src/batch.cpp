#include "batch.hh"



#include "debugmessage.hh"
#include "objectiveFunction.hh"
#include "appByWeight.hh"

#include <string>
#include <cmath>
#include <list>

/*
 * 		Name:					calculate_nu
 * 		Description: - it computes the nu indice for the first application
 * 								 - it computes the nu indices for all the other applications
 */

void
Batch::calculate_nu(optJrParameters &par)
{

  std::string debugMsg;
  double w1;
  double chi_c_1;
  double csi_1;
  int N = par.get_number(); // N is the number of cores
  double csi;

  int minCapacity= 0;

  debugMsg="Calculate nu indices and bounds for each application"; debugMessage(debugMsg, par);
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
    minCapacity+= it->V;
  }

  if(N<minCapacity)
  {
    std::cout << "<fatal error>: The number of cores is lower than the minimum capacity requirement "<<std::endl;
    std::cout << "minimim capacity: "<<minCapacity<<std::endl;
    std::cout << "number of cores: "<<N<<std::endl;
    exit(-1);
  }
  N = N - minCapacity; // N now is the number of cores available to exchange
  debugMsg="available cores to exchange ="+ std::to_string(N); debugMessage(debugMsg, par);


  /*
   *  COMPUTE NU_1
   */
  double tot = 0;
  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it==APPs.begin()) // First app only
    {
      w1 = it->w;
      chi_c_1 = it->chi_C;
      csi_1 = std::max(it->M/it->m, it->V/it->v);
      //printf("Calculate_nu first app: %s w1 %d chi_c %lf chi_c_1%lf\n", it->app_id, w1, chi_c_1, csi_1);
    }
    else //Any other row (compute tot)
    {
      csi = std::max(it->M/it->m, it->V/it->v);
      it->term_i = sqrt( (it->w/w1)*(it->chi_C/chi_c_1)*(csi_1/csi));
      tot = tot + it->term_i;
      // printf("Calculate_nu  Other rows: %s w %d csi %lf tot %lf\n", it->app_id, it->w, csi, tot);
    }
  }

  double nu_1 = N/(1 + tot);
  //printf("nu_1=%lf\n", nu_1);
  //tot = 0;
  //double term_j;



  /* NB: perchè ricalcola tot??? (uguale?) Penso sia superfluo
  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it!=APPs.begin())
    {
      csi = std::max(current->M/current->m, current->V/current->v);
      term_j = sqrt((current->w/w1)*(current->chi_C/chi_c_1)*(csi_1/csi));
      tot = tot+ term_j;
    }
  }
  */


  /*
   * COMPUTE NU_i (!=NU_1) AND ASSIGN VALUES
   */

  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it==APPs.begin()) it->nu_d = nu_1;
    else
    {
      it->nu_d = (it->term_i/(1 + tot))*N; //NB: potrei fare semplicemente nu_1*term_i, no?
      //printf("\nTERM app %s %lf tot %lf\n", it->app_id, it->term_i, (1 + tot) );
    }
    //printf("NU_i%lf nu1 %lf\n", it->nu_d, nu_1);
    it->currentCores_d = it->nu_d;  //NB:  currentCores_d ?? che roba è?? a che serve???
    std::cout<<"App ID: "<<it->app_id<<", NU: "<<it->nu_d<<std::endl;



      /*
       *  COMPUTE ALPHA AND BETA
       */

    //TODO: Sistema queste due (erano computeAlpha, computeBeta)
    it->beta = ((double) it->sAB.vec[1].nCores) / (it->sAB.vec[0].nCores - it->sAB.vec[1].nCores) * (((double) it->sAB.vec[0].nCores)/it->sAB.vec[1].nCores * it->sAB.vec[0].R - it->sAB.vec[1].R);
    it->alpha =it->sAB.vec[1].nCores * (it->sAB.vec[1].R - it->beta);

    //TODO: implementare come metodo classe application printRow(it, par)?;


  }
  debugMsg="end calculate nu"; debugMessage(debugMsg, par);


};


/*
 * Name:				initialize
 * Description			For each application, a base value for the objective function is calculated.
 */

void Batch::initialize(sConfiguration  &configuration, MYSQL *conn, optJrParameters &par)
{
	std::string debugMsg;

  debugMsg =  "Information: INITIALIZE baseFo for all the applications" ;debugMessage(debugMsg, par);
	for (auto it =APPs.begin(); it!= APPs.end(); it++)
	{
			it->mode = R_ALGORITHM;
			it->baseFO = it->ObjFunctionComponent(configuration, conn, par);
			it->initialBaseFO = it->baseFO;
			debugMsg = "INITIALIZE BASE FO for APP "+ it->session_app_id
                + " baseFO = " + std::to_string(it->baseFO)+"\n"; debugMessage(debugMsg, par);
	}

  debugMsg = "\n************ INITIALIZATION OBJECTIVE FUNCTION RESULTS ***************";
  for (auto it =APPs.begin(); it!= APPs.end(); it++)
	{

			debugMsg += "\nINITIALIZE BASE FO for APP "+ it->session_app_id
                + " baseFO = " + std::to_string(it->baseFO);
	}debugMessage(debugMsg, par);


};



/*
 * Name: fixInitialSolution
 * Description: It fixes the initial solution by reallocating the residual
 *              cores to the applications that may need more resources
 */

void Batch::fixInitialSolution(optJrParameters &par)
{

	int allocatedCores;
	appByWeight  LP ;
	int loopExit = 0;
	int residualCores;
  std::string debugMsg;
	int N = par.get_number();

	allocatedCores = 0;

	for(auto it = APPs.begin(); it!=APPs.end(); ++it)
	{

		it->currentCores_d = std::max(((int)(it->currentCores_d / it->V)) * it->V,it->V);
		if (it->currentCores_d > it->bound)
			it->currentCores_d = it->bound;
		else
			{
				debugMsg= "adding " + it->session_app_id + " to ApplicationPointers"; debugMessage(debugMsg, par);
        addApplicationPointer(LP, *it);
			}

		allocatedCores+= it->currentCores_d;
		debugMsg =  "fixInitialSolution FIXING CORES "+  it->session_app_id
                + " cores: " + std::to_string(it->currentCores_d); debugMessage(debugMsg, par);
	}

	debugMsg= "fixInitialSolution: allocatedCores "+ std::to_string(allocatedCores); debugMessage(debugMsg, par);



	residualCores = N - allocatedCores;
  std::cout << "RESIDUAL CORES: "<<residualCores<<std::endl;
	int addedCores;


  //Scorro l'elenco delle app "sofferenti " in ordine di peso, finchè ho cores disponibili

  auto it=LP.begin();
	while (!loopExit&& (residualCores>0))
	{
		if (it==LP.end() ) loopExit = 1;
		else
		{

			int potentialDeltaCores=((int)(residualCores / (*it)->V ) )* (*it)->V;

			if (((*it)->currentCores_d + potentialDeltaCores) > (*it)->bound){
				addedCores = (*it)->bound - (*it)->currentCores_d ;
			  (*it)->currentCores_d = (*it)->bound;

			}
			else{
				(*it)->currentCores_d = (*it)->currentCores_d + potentialDeltaCores;
				addedCores=potentialDeltaCores;
			}

			if ((*it)->currentCores_d == 0)
			{
				printf("\nFatal Error: FixInitialSolution: app %s has %d cores after fix\n", (*it)->session_app_id, (*it)->currentCores_d);
				exit(-1);
			}
			if (addedCores > 0)
			{

				debugMsg="adding cores to App " + (*it)->session_app_id + " added Cores: " +  std::to_string(addedCores) ;debugMessage(debugMsg, par);
				debugMsg," application_id " + (*it)->session_app_id + " new cores " + std::to_string((int)(*it)->currentCores_d) + " moved cores "+ std::to_string(addedCores) ;debugMessage(debugMsg, par);
				residualCores = residualCores - addedCores;
			}
			it++;
		}

		if (residualCores == 0) loopExit = 1;
	}


  debugMsg= " \n\n   ********************  FIXED INITIAL SOLUTION ***********************";debugMessage(debugMsg, par);
	for (auto it = APPs.begin(); it!=APPs.end(); ++it)
  {
    debugMsg = " Application " + it->app_id + ",  w = " + std::to_string(it->w)
             + " ncores = " + std::to_string(it->currentCores_d); debugMessage(debugMsg, par);
  }

}






/*
 * Name: approximatedLoop
 * Description: It estimates the objective function for each move. The candidate applications for which the move is profitable is stored in a sCandidate object
 */

sCandidates Batch::approximatedLoop( int &iteration, optJrParameters &par ) //NB: restituisce per copia! pesante??
{

  std::string debugMsg;


	if (APPs.empty())
	{
		printf("Error: approximatedLoop: NO Applications\n");
		exit(-1);
	}

	int nCoreMov;
	double DELTAVM_i;
	double DELTAVM_j;
	double DELTA_fo_App_i, DELTA_fo_App_j;
  sCandidates  sCandidateApproximated ;


	debugMsg= "Approximated iterated loop"; debugMessage(debugMsg, par);
  auto application_i=APPs.begin();
  while (application_i!=APPs.end())
  {

		auto application_j = application_i;
		while (application_j != APPs.end())
		{
			if (application_i->session_app_id!=application_j->session_app_id)
			{
				debugMsg="Comparing " + application_i->session_app_id + " with " + application_j->session_app_id;debugMessage(debugMsg, par);

				nCoreMov = std::max(application_i->V, application_j->V);

				DELTAVM_i = nCoreMov/application_i->V; debugMsg = "app " + application_i->session_app_id + " DELTAVM_i " +  std::to_string(DELTAVM_i); debugMessage(debugMsg, par);
				DELTAVM_j = nCoreMov/application_j->V; debugMsg = "app " + application_j->session_app_id + " DELTAVM_j " +  std::to_string(DELTAVM_j); debugMessage(debugMsg, par);

				// Change the currentCores, but rollback later
				int deltaNCores_i = DELTAVM_i * application_i->V;
				int deltaNCores_j = DELTAVM_j * application_j->V;
				application_i->currentCores_d = application_i->currentCores_d + deltaNCores_i;
				application_j->currentCores_d = application_j->currentCores_d - deltaNCores_j;


				debugMsg= "After cores exchange: app " + application_i->session_app_id + " currentCores " + std::to_string((int)application_i->currentCores_d);debugMessage(debugMsg, par);
        debugMsg= "After cores exchange: app " + application_j->session_app_id + " currentCores " + std::to_string((int)application_j->currentCores_d);debugMessage(debugMsg, par);


				if (application_i->currentCores_d > 0 && application_j->currentCores_d > 0)
				{
					DELTA_fo_App_i = ObjFun::ObjFunctionComponentApprox(*application_i, par) - application_i->baseFO;
					debugMsg = "app " + application_i->session_app_id + "DELTA_fo_App_i " + std::to_string(DELTA_fo_App_i);debugMessage(debugMsg, par);

					DELTA_fo_App_j = ObjFun::ObjFunctionComponentApprox(*application_j, par) - application_j->baseFO;
          debugMsg = "app " + application_j->session_app_id + "DELTA_fo_App_j " + std::to_string(DELTA_fo_App_j);debugMessage(debugMsg, par);

					if ((DELTA_fo_App_i + DELTA_fo_App_j < 0))
					{
						addCandidate(sCandidateApproximated,
									*application_i ,
									*application_j ,
									application_i->currentCores_d,
									application_j->currentCores_d,
									DELTA_fo_App_i + DELTA_fo_App_j,
									DELTAVM_i,
									DELTAVM_j
									);

					}


				}

				application_i->currentCores_d = application_i->currentCores_d - DELTAVM_i*application_i->V;
				application_j->currentCores_d = application_j->currentCores_d + DELTAVM_j*application_j->V;
			}
		application_j++;
		}
  application_i++;
	}



  iteration= sCandidateApproximated.size();

	return sCandidateApproximated;

}
