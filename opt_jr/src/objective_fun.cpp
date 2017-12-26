#include "objective_fun.hh"


#include <string>

#include "invoke_predictor.hh"

/*
 * 		Name:						component
 * 		Output parameters:			double The contribution to the calculation of the objective function
 * 		Description:				Currently, only one method is supported. Note that the algorithm's choice is stored in the "mode" field
 * 									of the application structure.
 *
 */

double Objective_fun::component(Configuration &configuration, MYSQL *conn, Application  &app, Opt_jr_parameters &par)
{

	std::string debugMsg;
	double output;

	/* The memory pattern can be anything such as "*" */
	app.set_R_d( atof(invoke_predictor( configuration, conn, 1, app.get_currentCores_d(), (char*)"*", app.get_dataset_size(),  const_cast<char*>((app.get_session_app_id()).c_str()),
													const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par,RESIDUAL_DAGSIM)));


	/* Determine how the obj function needs to be calculated */
	/*switch(app.get_mode())
	{*
		case R_ALGORITHM:*/
				debugMsg = "component W " + std::to_string(app.get_w()) + "   R_d " + std::to_string(app.get_R_d()) + "  D " + std::to_string(app.get_Deadline_d()); par.debug_message(debugMsg);
				if (app.get_R_d() > app.get_Deadline_d())
					output = app.get_w() * (app.get_R_d() - app.get_Deadline_d());
				else output = 0;
				debugMsg = "Compute FO for app " + app.get_session_app_id() + " currentCores_d " + std::to_string((int)app.get_currentCores_d()) + "  R " + std::to_string(app.get_R_d()) + " FO = "+  std::to_string(output); par.debug_message(debugMsg);
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
			printf("component: unknown case within Switch statement: get_mode() %d\n", app.get_mode());
			exit(-1);
			break;*
	}*/

	return output;
}



/**
  Name: component_approx
  Output parameters: a double The value of the approximated objective function
  Description It computes an approximation of the objective function (and update R_d)
 */

double Objective_fun::component_approx(Application &App, Opt_jr_parameters &par)//sApplication * pointer, struct Opt_jr_parameters par)
{

  std::string debugMsg;
	double output;


	App.set_R_d( App.get_alpha()/App.get_currentCores_d() + App.get_beta()); // update R_d

	/* Determine how the obj function needs to be calculated */

	debugMsg= "W " + std::to_string(App.get_w()) + " R_d " + std::to_string(App.get_R_d()) + " D " + std::to_string(App.get_Deadline_d()); par.debug_message(debugMsg);
	if (App.get_R_d() > App.get_Deadline_d())
		output = App.get_w() * (App.get_R_d() - App.get_Deadline_d());//NB: is it correct? very high values
	else output = 0;
		debugMsg = "Compute FO for app " +  App.get_session_app_id() + " currentCores_d"
                + std::to_string((int)App.get_currentCores_d()) + " R "+ std::to_string(App.get_R_d())
                + " FO=" + std::to_string(output); par.debug_message(debugMsg);

	return output;
}


/*
 * Name: global
 * Output parameters: double, the total value of objective function
 * Description:It calculates the value of the total objective function
 */
double Objective_fun::global(Configuration &configuration, MYSQL *conn, Batch & App_manager, Opt_jr_parameters &par)
{

	double sum = 0;


	for (auto it = App_manager.get_begin(); it!=App_manager.get_end(); ++it)
	{
		sum = sum + component(configuration, conn, *it, par);
	}

	if (double_compare(sum, 0) == 0)
	{
		std::string debugMsg="global: sum equal to zero"; par.debug_message(debugMsg);
		//exit(-1);
	}

	return sum;
}
