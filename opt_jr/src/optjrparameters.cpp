#include "optjrparameters.hh"
#include "optjrParam_helper.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// Constructor

optJrParameters::optJrParameters (char **args, int argc)
{

	if (argc != ARGS) Usage();
	for (int i = 0; i < ARGS; i++)
		if (strstr(args[i], FILENAME)) filename=parseArg(args[i], FILENAME, STRING);//strcpy(filename, parseArg(args[i], FILENAME, STRING));

		else if (strstr(args[i], NUM_N)) number = atoi(parseArg(args[i], NUM_N, NUMBER));

				else if (strstr(args[i], LIST_LIMIT)) K = atoi(parseArg(args[i], LIST_LIMIT, NUMBER));

					else if (strstr(args[i], DEBUG)) debug = atoi(parseArg(args[i], DEBUG, YES_NO));


							else if (strstr(args[i], SIMULATOR))
							{
									if (strcmp(parseArg(args[i], FILENAME, STRING), "dagSim") == 0) simulator = DAGSIM;
															else simulator = LUNDSTROM;
							}

								else if (strstr(args[i], GLOBAL_FO_CALCULATION)) globalFOcalculation = atoi(parseArg(args[i], GLOBAL_FO_CALCULATION, YES_NO));
								else if (strstr(args[i], MAX_ITERATIONS))
									maxIterations = atoi(parseArg(args[i], MAX_ITERATIONS, NUMBER));


}

/*
	Get Functions
*/
const std::string optJrParameters::get_filename()
{
		return filename;
}

const int optJrParameters::get_debug()
{
	return debug;
}

const int optJrParameters::get_cache()
{
	return cache;
}

const int optJrParameters::get_globalFOcalculation()
{
	return globalFOcalculation;
}

const int optJrParameters::get_K()
{
	return K;
}

const int optJrParameters::get_simulator()
{
	return simulator;
}

const int optJrParameters::get_number()
{
	return number;
}


const int optJrParameters::get_maxIteration()
{
	return maxIterations;
}
