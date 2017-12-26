#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <string>
#include <mysql.h>




/**
	In the Application class all the data of one application are stored;
	it's provided also with a method to evaluate hyperbolic interpolation for alpha and beta, and several get and set funtions.
*/
class Application{


	double ncores_old;   ///<Value used by compute_alpha_beta
	double ncores_new;   ///<Value used by compute_alpha_beta
	double R_old;        ///<Value used by compute_alpha_beta
	double R_new;        ///<Value used by compute_alpha_beta
	int bound_iterations=0;       ///< Index for Hyperbolic interpolation

	/* "Static" values */
	const std::string session_app_id; ///< Session identifier
	const std::string app_id; 		///<  Application identifier
	const double w; 			///<  Weight of the application
	double term_i;	///<  Used to calculate nu index
	const double chi_0; 	///< Machine learning parameter
	const double chi_C;		///< Machine learning parameter
	const double m;				///< Ram of a container for this application
	const double M;				///< Total Ram available at the YARN NodeManager
	const double V;				///< Total vCPUs available at the YARN NodeManager
	const double v;				///< vCPUs of a container for this application
	const double Deadline_d; ///< Deadline for the application
	const double csi;					///csi:= Min{M/m,V/v}
	const std::string stage; ///<  Application's stage (used in case of residual time)
	const int dataset_size;   ///< Size of the dataset


	/* Dynamic values */
	double nu_d;        ///< nu value
	int  currentCores_d;		///< Initialized to nu_i
	int  nCores_DB_d;			///< Initialized to the value from look-up table
	int bound;					///< Bound (number of cores)
	double R_d;					///< Value of R as per the predictor
	double baseFO;				///< base FO value (used to calculate the delta)
	int vm;						///< Read from OPTIMIZER_CONFIGURATION_TABLE
	double alpha;      ///< First parameter for Hyperbolic interpolation
	double beta;       ///< Second parameter for Hyperbolic interpolation


public:

	///Constructor expects all not editable values.
  Application(std::string session_app_id, std::string app_id, double w,
              double chi_0, double chi_C, double m, double M, double V,
              double v, double D, double csi, std::string St, int dataset_size);
	/// This function evaluates the Hyperbolic interpolation for alpha and beta (from the second time it is invoked).
	void compute_alpha_beta(int nCores_n, double R_n);


	/*
	GETTER FUNCTIONS
	*/
	const std::string get_session_app_id(){  return session_app_id;}
	const std::string get_app_id(){  return app_id;}
	const double get_w(){	  return w;}
	double get_term_i(){  return term_i;}
	const double get_chi_0(){ return chi_0;}
	const double get_chi_C(){ return chi_C;}
	const double get_m(){  return m;}
	const double get_M(){  return M;}
	const double get_v(){ return v;}
	const double get_V(){  return V;}
	const double get_Deadline_d(){  return Deadline_d;}
	const double get_csi(){  return csi;}
	const std::string get_stage(){  return stage;	}
	const int get_dataset_size()	{	return dataset_size;}
	const int get_currentCores_d(){return currentCores_d;}
	double get_nu_d(){return nu_d;}
	int get_nCores_DB_d(){	return nCores_DB_d;}
	int get_bound(){	return bound;}
	double get_R_d(){  return R_d;}
	double get_baseFO(){ return baseFO;}
	int get_vm(){	return vm;}
	double get_alpha(){ return alpha;}
	double get_beta(){ return beta;}

	/*
			SETTER FUNCTIONS
	*/
	void set_term_i(double t_i){	term_i=t_i;}
	void set_currentCores_d(int c_d){	currentCores_d=c_d;}
  void set_nu_d(double n){	nu_d=n;}
	void  set_nCores_DB_d(int n){	nCores_DB_d=n;	}
	void set_bound (int b){	bound= b;}
	void set_R_d(double r){	R_d=r;}
	void set_baseFO(double f){baseFO=f;}
	void set_vm(int v){vm=v;}


};

#endif
