#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <string>
#include <mysql.h>

#include "invokePredictor.hh"

#define HYP_INTERPOLATION_POINTS  2

#define WHOLE_EXECUTION_TIME 0 // used by obj function component
#define RESIDUAL_EXECUTION_TIME 1


#define R_ALGORITHM 0
#define CORES_ALGORITHM 1
#define NCORES_ALGORITHM 2


/**
	used for interpolation, included in sAlphaBetaManagement
*/

class slastSimulatorRun{
public:
	int nCores;
	double R;
};


/**
  class AlphaBetaManagement:  used for interpolation, included in Application class
 */
class sAlphaBetaManagement{
public:
  slastSimulatorRun vec[HYP_INTERPOLATION_POINTS];
  int index;

};




/**
	In the Application class are stored all the data of one application;
	it's provided also a method to evaluate the objective function of the application
*/
class Application{
public:


	/* Static values */

  int mode; 				///< How the objective function is calculated (currently redundant)
  std::string session_app_id; ///< Session identifier
  std::string app_id; 		///<  Application identifier
  double w; 			///<  Weight application
  double term_i;	///<  Used to calculate nu index
  double chi_0; 	///< Machine learning parameter
  double chi_C;		///< Machine learning parameter
  double m;
  double M;
  double V;
  double v;
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
  double R_bound_d;			///< Bound (R)
	double baseFO;				///< base FO value (used to calculate the delta)
	double initialBaseFO;		///< copy of base FO value (used to reset the value)
  float alpha;				///< First parameter for Hyperbolic interpolation
  float beta;					///< Second parameter for Hyperbolic interpolation
  sAlphaBetaManagement sAB;
  int boundIterations;		///< Metrics
  int vm;						///< Read from OPTIMIZER_CONFIGURATION_TABLE



	///Constructor expects all static values
  Application(std::string session_app_id, std::string app_id, double w,
              double chi_0, double chi_C, double m, double M, double V,
              double v, double D, double csi, std::string St, int DatasetSize);


	/// Evaluates objective function for the application
  double ObjFunctionComponent(sConfiguration &configuration, MYSQL *conn, optJrParameters &par);

};

#endif
