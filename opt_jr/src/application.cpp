#include "application.hh"


/**
Constructor:  expects all static values of the application
*/
Application::Application(std::string session_app_id, std::string app_id, double w,
            double chi_0, double chi_C, double m, double M, double V,
            double v, double D, double csi, std::string St, int dataset_size):

            session_app_id(session_app_id), app_id(app_id), w(w), chi_0(chi_0),
            chi_C(chi_C), m(m), M(M), V(V), v(v), Deadline_d(D), csi(csi), stage(St),
            dataset_size(dataset_size){};




/**
compute_alpha_beta evaluates the hyperbolic interpolation for alpha and beta
(from the second time it is invoked).
*/
void Application::compute_alpha_beta(int nCores_n, double R_n)
{
  if (bound_iterations==0)
  {
    R_new=R_n;
    ncores_new=nCores_n;
    bound_iterations++;
  }
  else
  {
    R_old=R_new;
    ncores_old=ncores_new;
    R_new=R_n;
    ncores_new=nCores_n;
    beta = (ncores_old*R_old - ncores_new*R_new)/ (ncores_old-ncores_new);
    alpha =(R_new - beta)*ncores_new;
    bound_iterations++;
  }
}
