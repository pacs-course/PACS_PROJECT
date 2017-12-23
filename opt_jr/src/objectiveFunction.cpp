#include "objectiveFunction.hh"


#include <string>
#include "debugmessage.hh"
#include "invokePredictor.hh"

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
	app.set_R_d( atof(invokePredictor( configuration, conn, 1, app.get_currentCores_d(), (char*)"*", app.get_dataset_size(),  const_cast<char*>((app.get_session_app_id()).c_str()),
													const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par,RESIDUAL_DAGSIM)));


	/* Determine how the obj function needs to be calculated */
	/*switch(app.get_mode())
	{*
		case R_ALGORITHM:*/
				debugMsg = "ObjFunctionComponent W " + std::to_string(app.get_w()) + "   R_d " + std::to_string(app.get_R_d()) + "  D " + std::to_string(app.get_Deadline_d()); debugMessage(debugMsg, par);
				if (app.get_R_d() > app.get_Deadline_d())
					output = app.get_w() * (app.get_R_d() - app.get_Deadline_d());
				else output = 0;
				debugMsg = "Compute FO for app " + app.get_session_app_id() + " currentCores_d " + std::to_string((int)app.get_currentCores_d()) + "  R " + std::to_string(app.get_R_d()) + " FO = "+  std::to_string(output); debugMessage(debugMsg, par);
			//break;
			/*
		case CORES_ALGORITHM:
			printf("Cores Algorithm\n");
				if (app.currentCores > app.newCores) output = 0;
				else output = app.get_w() * (app.Rnew - app.D);

			break;
		case NCORES_ALGORITHM:
			printf("NCores Algorithm\n");
				if (app.newCores >app.get_bound()) output = 0;
				else output = app.get_w() * app.R - app.R;
			break;
			*
		default:
			printf("ObjFunctionComponent: unknown case within Switch statement: get_mode() %d\n", app.get_mode());
			exit(-1);
			break;*
	}*/

	return output;
}



/**
  Name: ObjFunctionComponentApprox
  Output parameters: a double The value of the approximated objective function
  Description It computes an approximation of the objective function (and update R_d)
 */

double ObjFun::ObjFunctionComponentApprox(Application &App, optJrParameters &par)//sApplication * pointer, struct optJrParameters par)
{

  std::string debugMsg;
	double output;


	App.set_R_d( App.get_alpha()/App.get_currentCores_d() + App.get_beta()); // update R_d

	/* Determine how the obj function needs to be calculated */

	debugMsg= "W " + std::to_string(App.get_w()) + " R_d " + std::to_string(App.get_R_d()) + " D " + std::to_string(App.get_Deadline_d()); debugMessage(debugMsg, par);
	if (App.get_R_d() > App.get_Deadline_d())
		output = App.get_w() * (App.get_R_d() - App.get_Deadline_d());//NB: is it correct? very high values
	else output = 0;
		debugMsg = "Compute FO for app " +  App.get_session_app_id() + " currentCores_d"
                + std::to_string((int)App.get_currentCores_d()) + " R "+ std::to_string(App.get_R_d())
                + " FO=" + std::to_string(output); debugMessage(debugMsg, par);

	return output;
}


/*
 * Name: ObjFunctionGlobal
 * Output parameters: double, the total value of objective function
 * Description:It calculates the value of the total objective function
 */
double ObjFun::ObjFunctionGlobal(sConfiguration &configuration, MYSQL *conn, Batch & App_manager, optJrParameters &par)
{

	double sum = 0;


	for (auto it = App_manager.get_begin(); it!=App_manager.get_end(); ++it)
	{
		sum = sum + ObjFunctionComponent(configuration, conn, *it, par);
	}

	if (doubleCompare(sum, 0) == 0)
	{
		std::string debugMsg="ObjFunctionGlobal: sum equal to zero"; debugMessage(debugMsg, par);
		//exit(-1);
	}

	return sum;
}
