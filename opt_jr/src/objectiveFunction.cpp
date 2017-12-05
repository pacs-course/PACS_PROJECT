#include "objectiveFunction.hh"


#include <string>
#include "debugmessage.hh"

/*
 * 		Name:						ObjFunctionComponent
 * 		Output parameters:			double The contribution to the calculation of the objective function
 * 		Description:				Currently, only one method is supported. Note that the algorithm's choice is stored in the "mode" field
 * 									of the application structure.
 *
 */

double ObjFun::ObjFunctionComponent(sConfiguration &configuration, MYSQL *conn, Application  &app, optJrParameters &par)
{

	std::string debugMsg;
	double output;

	/* The memory pattern can be anything such as "*" */
	app.R_d = atof(invokePredictor( configuration, conn, 1, app.currentCores_d, "*", app.datasetSize,  const_cast<char*>((app.session_app_id).c_str()),
													const_cast<char*>((app.app_id).c_str()), const_cast<char*>((app.stage).c_str()), par,RESIDUAL_EXECUTION_TIME));

	//printf("ObjFunctionComponent: App_id %s w %f R %d D %d nCores %d newCores %d\n",app.app_id, app.w, app.R, app.D, app.cores, app.newCores);

	/* Determine how the obj function needs to be calculated */
	switch(app.mode)
	{
		case R_ALGORITHM:
				debugMsg = "ObjFunctionComponent W " + std::to_string(app.w) + "R_d" + std::to_string(app.R_d) + "D" + std::to_string(app.Deadline_d); debugMessage(debugMsg, par);
				if (app.R_d > app.Deadline_d)
					output = app.w * (app.R_d - app.Deadline_d);
				else output = 0;
				debugMsg = "Compute FO for app " + app.session_app_id + " currentCores_d " + std::to_string((int)app.currentCores_d) + "  R " + std::to_string(app.R_d) + " FO = "+  std::to_string(output); debugMessage(debugMsg, par);
			break;
			/*
		case CORES_ALGORITHM:
			printf("Cores Algorithm\n");
				if (app.currentCores > app.newCores) output = 0;
				else output = app.w * (app.Rnew - app.D);

			break;
		case NCORES_ALGORITHM:
			printf("NCores Algorithm\n");
				if (app.newCores >app.bound) output = 0;
				else output = app.w * app.R - app.R;
			break;
			*/
		default:
			printf("ObjFunctionComponent: unknown case within Switch statement: mode %d\n", app.mode);
			exit(-1);
			break;
	}

	return output;
}



/*
 * Name: ObjFunctionComponentApprox
 * Output parameters: a double The value of the the approximated objective function
 * Description It computes an approximation of the objective function (and update R_d)
 */

double ObjFun::ObjFunctionComponentApprox(Application &App, optJrParameters &par)//sApplication * pointer, struct optJrParameters par)
{

  std::cout << "  hello from ObjFunctionComponentApprox "<<std::endl;
  std::string debugMsg;
	//char debugMsg[DEBUG_MSG];
	double output;


	App.R_d = App.alpha/App.currentCores_d + App.beta; // update R_d

	/* Determine how the obj function needs to be calculated */

	debugMsg= "W " + std::to_string(App.w) + " R_d " + std::to_string(App.R_d) + " D " + std::to_string(App.Deadline_d); debugMessage(debugMsg, par);
	if (App.R_d > App.Deadline_d)
		output = App.w * (App.R_d - App.Deadline_d);//NB: is it correct? very high values
	else output = 0;
		debugMsg = "Compute FO for app " +  App.session_app_id + " currentCores_d"
                + std::to_string((int)App.currentCores_d) + " R "+ std::to_string(App.R_d)
                + " FO=" + std::to_string(output); debugMessage(debugMsg, par);

	return output;
}
