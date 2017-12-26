
#include "opt_jr_parameters.hh"
#include "opt_jr_parameters_helper.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


/// The constructor takes in input all the input from command line.
Opt_jr_parameters::Opt_jr_parameters (char **args, int argc)
{

	if (argc != ARGS) Usage(argc);
	for (int i = 0; i < ARGS; i++)
	{
		if (strstr(args[i], FILENAME)) filename=parseArg(args[i], (char*)FILENAME, STRING, argc);//strcpy(filename, parseArg(args[i], FILENAME, STRING));

		else if (strstr(args[i], NUM_N)) number = atoi(parseArg(args[i], (char*)NUM_N, NUMBER, argc));

		else if (strstr(args[i], LIST_LIMIT)) K = atoi(parseArg(args[i], (char*)LIST_LIMIT, NUMBER, argc));

		else if (strstr(args[i], DEBUG)) debug = atoi(parseArg(args[i], (char*)DEBUG, YES_NO, argc));
		else if (strstr(args[i], GLOBAL_FO_CALCULATION)) global_FO_calculation = atoi(parseArg(args[i], (char*)GLOBAL_FO_CALCULATION, YES_NO, argc));
		else if (strstr(args[i], MAX_ITERATIONS))
		max_iterations = atoi(parseArg(args[i], (char*)MAX_ITERATIONS, NUMBER, argc));
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


/// set_number_of_threads sets the number_of_threads: it looks in configuration file (0== "no parallelization")

void Opt_jr_parameters::set_number_of_threads(Configuration &configuration)
{
	/* Read the threads number */
	number_of_threads = stoi(configuration["THREADS_NUMBER"]);
}



/// If debug is activated the message in string is printed.
void Opt_jr_parameters::debug_message(std::string &string)
{
	if (debug == 1)
	{
		std::cout<<"<debug message>:"<<string<<std::endl;
	}
}
