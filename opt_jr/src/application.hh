#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <string>
#include <mysql.h>

#include "invokePredictor.hh"

//#define HYP_INTERPOLATION_POINTS  2




#define R_ALGORITHM 0
/*#define CORES_ALGORITHM 1
#define NCORES_ALGORITHM 2*/


/*



class slastSimulatorRun{
public:
	int nCores;
	double R;
};




class sAlphaBetaManagement{
public:
  slastSimulatorRun vec[HYP_INTERPOLATION_POINTS];
  int index;

};
*/




/**
	In the Application class all the data of one application are stored ;
	it's provided also a method to evaluate Hyperbolic interpolation for alpha and beta.
*/
class Application{
	/*
	int nCores;
	double R;
	slastSimulatorRun vec[HYP_INTERPOLATION_POINTS];
	int index;*/
	double nCores_old;
	double nCores_new;
	double R_old;
	double R_new;

public:


	/* Static values */

  int mode=R_ALGORITHM; 				///< How the objective function is calculated (currently redundant)
  std::string session_app_id; ///< Session identifier
  std::string app_id; 		///<  Application identifier
  double w; 			///<  Weight application
  double term_i;	///<  Used to calculate nu index
  double chi_0; 	///< Machine learning parameter
  double chi_C;		///< Machine learning parameter
  double m;				///< Ram of a container for this application
  double M;				///< Total Ram available at the YARN NodeManager
  double V;				///< Total vCPUs available at the YARN NodeManager
  double v;				///< vCPUs of a container for this application
  double Deadline_d; ///< Deadline for the application
  double csi;
  std::string stage; ///<  Application's stage (used in case of residual time)
  int datasetSize;   ///< Size of the dataset

  /* Dynamic values */

  double nu_d; ///< nu value
  int  currentCores_d;		///< Initialized to nu_i
  int  nCores_DB_d;			///< Initialized to the value from look-up table
  int bound;					///< Bound (number of cores)
  double R_d;					///< Value of R as per the predictor
  //double R_bound_d;			///< Bound (R)
	double baseFO;				///< base FO value (used to calculate the delta)
	double initialBaseFO;		///< copy of base FO value (used to reset the value)
  //float alpha;
  //float beta;
//  sAlphaBetaManagement sAB;
  int boundIterations;		///< Metrics
  int vm;						///< Read from OPTIMIZER_CONFIGURATION_TABLE

	double alpha;      ///< First parameter for Hyperbolic interpolation
	double beta;       ///< Second parameter for Hyperbolic interpolation
	int index=0;       ///< Index for Hyperbolic interpolation



	///Constructor expects all static values.
  Application(std::string session_app_id, std::string app_id, double w,
              double chi_0, double chi_C, double m, double M, double V,
              double v, double D, double csi, std::string St, int DatasetSize);
	/// This function evaluates the Hyperbolic interpolation for alpha and beta (from the second time it is invoked).
	void computeAlphaBeta(int nCores_n, double R_n);

};

#endif
