#include "application.hh"


/*
Constructor:  expects all static values of the application
*/
Application::Application(std::string session_app_id, std::string app_id, double w,
            double chi_0, double chi_C, double m, double M, double V,
            double v, double D, double csi, std::string St, int dataset_size):

            session_app_id(session_app_id), app_id(app_id), w(w), chi_0(chi_0),
            chi_C(chi_C), m(m), M(M), V(V), v(v), Deadline_d(D), csi(csi), stage(St),
            dataset_size(dataset_size){};

void Application::computeAlphaBeta(int nCores_n, double R_n)
{
  if (boundIterations==0)
  {
    R_new=R_n;
    nCores_new=nCores_n;
    boundIterations++;
  }
  else
  {
    R_old=R_new;
    nCores_old=nCores_new;
    R_new=R_n;
    nCores_new=nCores_n;
    beta = (nCores_old*R_old - nCores_new*R_new)/ (nCores_old-nCores_new);
    alpha =(R_new - beta)*nCores_new;
    boundIterations++;
  }
}
