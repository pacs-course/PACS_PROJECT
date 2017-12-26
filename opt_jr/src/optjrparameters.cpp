
#include "optjrparameters.hh"
#include "optjrParam_helper.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


/*
 Constructor
 */

OPT_JR_parameters::OPT_JR_parameters (char **args, int argc)
{

	if (argc != ARGS) Usage(argc);
	for (int i = 0; i < ARGS; i++)
	{
		if (strstr(args[i], FILENAME)) filename=parseArg(args[i], (char*)FILENAME, STRING, argc);//strcpy(filename, parseArg(args[i], FILENAME, STRING));

		else if (strstr(args[i], NUM_N)) number = atoi(parseArg(args[i], (char*)NUM_N, NUMBER, argc));

				else if (strstr(args[i], LIST_LIMIT)) K = atoi(parseArg(args[i], (char*)LIST_LIMIT, NUMBER, argc));

					else if (strstr(args[i], DEBUG)) debug = atoi(parseArg(args[i], (char*)DEBUG, YES_NO, argc));


							else if (strstr(args[i], SIMULATOR))
							{
									if (strcmp(parseArg(args[i], (char*)FILENAME, STRING, argc), "dagSim") == 0) simulator = DAGSIM;
															else simulator = LUNDSTROM;
							}

								else if (strstr(args[i], GLOBAL_FO_CALCULATION)) globalFOcalculation = atoi(parseArg(args[i], (char*)GLOBAL_FO_CALCULATION, YES_NO, argc));
								else if (strstr(args[i], MAX_ITERATIONS))
									maxIterations = atoi(parseArg(args[i], (char*)MAX_ITERATIONS, NUMBER, argc));
								else if (strstr(args[i], CACHE))
									cache = atoi(parseArg(args[i], (char*)CACHE, YES_NO, argc));
								else if (strstr(args[i], SEARCH_TYPE))
									search_type = parseArg(args[i], (char*)SEARCH_TYPE, STRING, argc);
		}
		if (K!=0 && (search_type=="s"||search_type=="S"))
		{
			std::cout<< "If search-type is sequencial (-st=s/S) it's necessary to have -k=0"<<std::endl;
			exit(-1);
		}
		if(search_type!="s" && search_type!="S" && search_type!="a" &&search_type!="A")
		{
			std::cout << "Unknown search-type; -st option should be: -st=<a/A|s/S>"<<std::endl;
			exit(-1);
		}



}


void OPT_JR_parameters::set_numberOfThreads(Configuration &configuration)
{
	/* Read the threads number */
	numberOfThreads = stoi(configuration["THREADS_NUMBER"]);
}

void OPT_JR_parameters::debugMessage(std::string &string)
{
	if (debug == 1)
  {
	  std::cout<<"<debug message>:"<<string<<std::endl;
  }
}
