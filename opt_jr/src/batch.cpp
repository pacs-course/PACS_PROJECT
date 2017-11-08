#include "batch.hh"
#include "debugmessage.hh"

#include <string>
#include <cmath>



void
Batch::calculate_nu(optJrParameters &par)
{

  std::string debugMsg;
  int w1;
  double chi_c_1;
  double csi_1;
  int N = par.get_number();
  double csi;

  int minCapacity= 0;

  debugMsg="Calculate nu indices and bounds for each application"; debugMessage(debugMsg, par);
  if (APPs.begin()==APPs.end())
  {
    printf("Fatal error: There are not Applications\n");
    exit(-1);
  }

  // compute the minimum capacity to execute the applications
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
  debugMsg="available cores ="+ std::to_string(N); debugMessage(debugMsg, par);


  // COMPUTE NU_1
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
      it->term_i = sqrt( ((double)it->w/w1)*(it->chi_C/chi_c_1)*(csi_1/csi));
      tot = tot + it->term_i;
      // printf("Calculate_nu  Other rows: %s w %d csi %lf tot %lf\n", it->app_id, it->w, csi, tot);
    }
  }

  double nu_1 = N/(1 + tot);
  //printf("nu_1=%lf\n", nu_1);
  //tot = 0;
  //double term_j;



  /* NB: perchè ricalcola tot??? (uguale?)
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

  // Calculate nu_i (!=nu_1) and assign values
  for (auto it=APPs.begin(); it!=APPs.end(); it++)
  {
    if (it==APPs.begin()) it->nu_d = nu_1;
    else
    {
      it->nu_d = (it->term_i/(1 + tot))*N; //NB: potrei fare semplicemente nu_1*term_i, no?
      //printf("\nTERM app %s %lf tot %lf\n", it->app_id, it->term_i, (1 + tot) );
    }
    //printf("NU_i%lf nu1 %lf\n", it->nu_d, nu_1);
    it->currentCores_d = it->nu_d;
    std::cout<<"App ID: "<<it->app_id<<", NU: "<<it->nu_d<<std::endl;
    //NB:  currentCores_d ?? che roba è?? a che serve???


    //TODO: Sistema queste due (erano computeAlpha, computeBeta)
    it->beta = ((double) it->sAB.vec[1].nCores) / (it->sAB.vec[0].nCores - it->sAB.vec[1].nCores) * (((double) it->sAB.vec[0].nCores)/it->sAB.vec[1].nCores * it->sAB.vec[0].R - it->sAB.vec[1].R);
    it->alpha =it->sAB.vec[1].nCores * (it->sAB.vec[1].R - it->beta);

    //printf("App %s alpha = %lf beta = %lf\n", it->app_id, it->alpha, it->beta);
    //printf(" R %lf nCores %d\n", it->sAB.vec[0].R, it->sAB.vec[0].nCores);
    //printf(" R %lf nCores %d\n", it->sAB.vec[1].R, it->sAB.vec[1].nCores);

    //current->currentCores_d = current->bound_d;

    //TODO: implementare come metodo classe application printRow(it, par)?;


  }
  debugMsg="end calculate nu";debugMessage(debugMsg, par);


};
