#include "objective_fun.hh"


#include <string>

#include "invoke_predictor.hh"


/**
component evaluates the contribution to the calculation of the objective function of one application.
*/
double Objective_fun::component(Configuration &configuration, MYSQL *conn, Application  &app, Opt_jr_parameters &par)
{

	std::string debugMsg;
	double output;

	/* The memory pattern can be anything such as "*" */
	app.set_R_d( atof(invoke_predictor( configuration, conn, 1, app.get_currentCores_d(), (char*)"*", app.get_dataset_size(),  const_cast<char*>((app.get_session_app_id()).c_str()),
	const_cast<char*>((app.get_app_id()).c_str()), const_cast<char*>((app.get_stage()).c_str()), par,RESIDUAL_DAGSIM)));

	debugMsg = "component W " + std::to_string(app.get_w()) + "   R_d " + std::to_string(app.get_R_d()) + "  D " + std::to_string(app.get_Deadline_d()); par.debug_message(debugMsg);
	if (app.get_R_d() > app.get_Deadline_d())
	output = app.get_w() * (app.get_R_d() - app.get_Deadline_d());
	else output = 0;
	debugMsg = "Compute FO for app " + app.get_session_app_id() + " currentCores_d " + std::to_string((int)app.get_currentCores_d()) + "  R " + std::to_string(app.get_R_d()) + " FO = "+  std::to_string(output); par.debug_message(debugMsg);


	return output;
}



/**
component_approx computes an approximation of the objective function (and update R_d).
*/
double Objective_fun::component_approx(Application &App, Opt_jr_parameters &par)//sApplication * pointer, struct Opt_jr_parameters par)
{

  std::string debugMsg;
	double output;


	App.set_R_d( App.get_alpha()/App.get_currentCores_d() + App.get_beta()); // update R_d

	debugMsg= "W " + std::to_string(App.get_w()) + " R_d " + std::to_string(App.get_R_d()) + " D " + std::to_string(App.get_Deadline_d()); par.debug_message(debugMsg);
	if (App.get_R_d() > App.get_Deadline_d())
		output = App.get_w() * (App.get_R_d() - App.get_Deadline_d());//NB: is it correct? very high values
	else output = 0;
		debugMsg = "Compute FO for app " +  App.get_session_app_id() + " currentCores_d"
                + std::to_string((int)App.get_currentCores_d()) + " R "+ std::to_string(App.get_R_d())
                + " FO=" + std::to_string(output); par.debug_message(debugMsg);

	return output;
}


/**
global computes the value of the total objective function.
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
	}

	return sum;
}
