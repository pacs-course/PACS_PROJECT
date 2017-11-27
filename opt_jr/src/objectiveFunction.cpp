#include "objectiveFunction.hh"


#include <string>
#include "debugmessage.hh"


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
		output = App.w * (App.R_d - App.Deadline_d);
	else output = 0;
		debugMsg = "Compute FO for app " +  App.session_app_id + " currentCores_d"
                + std::to_string((int)App.currentCores_d) + " R "+ std::to_string(App.R_d)
                + " FO=" + std::to_string(output); debugMessage(debugMsg, par);

	return output;
}
