#include "application.hh"

Application::Application(std::string session_app_id, std::string app_id, double w,
            double chi_0, double chi_C, double m, double M, double V,
            double v, double D, double csi, std::string St, int DatasetSize):

            session_app_id(session_app_id), app_id(app_id), w(w), chi_0(chi_0),
            chi_C(chi_C), m(m), M(M), V(V), v(v), Deadline_d(D), csi(csi), stage(St),
            datasetSize(DatasetSize){};
