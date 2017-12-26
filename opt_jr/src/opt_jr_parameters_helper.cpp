#include "opt_jr_parameters_helper.hh"



/**
Usage explains the usage of the OPT_JR_CPP program.
*/
void Usage(int argc)
{
		printf("Arguments expected %d, actual arguments number was %d\n", ARGS, argc);
    	printf("Usage:\n");
    	printf("./OPT_JR -f=<filename.csv> -n=<N> -k=<MaximumCandidates> -d=<Y/y|N/n> -c=<Y/y|N/n> -s=<dagSim|lundstrom> -i=<iterations> -g=<Y/y|N/n>\n");
    	printf("where:\n");
    	printf("<filename.csv> is the csv file (including the input values) under $UPLOAD_HOME in wsi_config.xml;\n");
    	printf("<N> is the total number of cores;\n");
    	printf("<Limit> is the maximum number of considered candidates (if equal to 0, all the candidates are considered).\n");
    	printf("-d represents debug (on/off)\n");
    	printf("-c represents cache (on/off)\n");
    	printf("-i represents the maximum number of iterations\n");
    	printf("Example:\n");
    	printf("./OPT_JR -f=\"Test3.csv\" -n=220 -k=0 -c=y -d=Y -s=dagSim -i=10 -g=y\n");
    	printf("OPT_JR is executed on a file Test3.csv, the total number of cores is 220, all the candidates are considered, the predictor used is dagSim, "
    			"the maximum number of iterations is 10, the cache is enabled, the calculation of the globabl objective function is enabled\n");
    	exit(-1);
    }


/*
Function to parse argument from command line;
Invoked by Opt_jr_parameters constructor
*/

char* parseArg(char* string, char* gap, int type, int argc)
{

        switch(type)
        {
          case NUMBER:
          case STRING:
            return string + strlen(gap);
            break;

          case YES_NO:

            if (strstr(string + strlen(gap), "y") || strstr(string + strlen(gap), "Y")) return (char*)"1";

            else if (strstr(string + strlen(gap), "n") || strstr(string + strlen(gap), "N")) return (char*)"0";

              else Usage(argc);

            break;

          default:

            printf("Fatal Error: parseArg: option not recognized\n");
            exit(-1);
        }

        return NULL;

}
