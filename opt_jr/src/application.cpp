#include "application.hh"


/*
Constructor:  expects all static values of the application
*/
Application::Application(std::string session_app_id, std::string app_id, double w,
            double chi_0, double chi_C, double m, double M, double V,
            double v, double D, double csi, std::string St, int DatasetSize):

            session_app_id(session_app_id), app_id(app_id), w(w), chi_0(chi_0),
            chi_C(chi_C), m(m), M(M), V(V), v(v), Deadline_d(D), csi(csi), stage(St),
            datasetSize(DatasetSize){};



/*
  		Name:						ObjFunctionComponent
  		Description:		Evaluates the contribution to the calculation of the objective function.
 	                    Currently, only one method is supported. Note that the algorithm's choice is stored in the "mode" field
  									  of the application structure.
 */

double Application::ObjFunctionComponent(sConfiguration &configuration, MYSQL *conn, optJrParameters &par)
{

	std::string debugMsg;
	double output;

	/* The memory pattern can be anything such as "*" */
	R_d = atof(invokePredictor( configuration, conn, 1, currentCores_d,(char*) "*", datasetSize, const_cast<char*>((session_app_id).c_str()),
													const_cast<char*>((app_id).c_str()), const_cast<char*>((stage).c_str()), par,RESIDUAL_EXECUTION_TIME));


	/* Determine how the obj function needs to be calculated */
	switch(mode)
	{
		case R_ALGORITHM:
				debugMsg="ObjFunctionComponent W " + std::to_string(w) + " R_d "+ std::to_string(R_d)
                + " D "+ std::to_string(Deadline_d)  ;debugMessage(debugMsg, par);
				if (R_d > Deadline_d)
					output = w * (R_d - Deadline_d);
				else output = 0;
				debugMsg="Compute FO for app " + session_app_id + " currentCores_d " + std::to_string((int)currentCores_d)
                  + " R " + std::to_string(R_d); debugMessage(debugMsg, par);
			break;
			/*
		case CORES_ALGORITHM:
			printf("Cores Algorithm\n");
				if (pointer->currentCores > pointer->newCores) output = 0;
				else output = pointer->w * (pointer->Rnew - pointer->D);

			break;
		case NCORES_ALGORITHM:
			printf("NCores Algorithm\n");
				if (pointer->newCores >pointer->bound) output = 0;
				else output = pointer->w * pointer->R - pointer->R;
			break;
      */
		default:
			printf("ObjFunctionComponent: unknown case within Switch statement: mode %d\n", mode);
			exit(-1);
			break;
	}

	return output;
}
