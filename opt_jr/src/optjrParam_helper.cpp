#include "optjrParam_helper.hh"

/*
 Explain usage of the OPT_JR_CPP program
 Invoked if the number of parameters send at command line is incorrect
*/

void Usage()
{

    	printf("Usage:\n");
    	printf("./optimize -f <filename.csv> -n <N> -k <Limit> -d <dY/y|N/n> -c <Y/y|N/n> -s <dagSim|lundstrom> -i <iterations>\n");
    	printf("where:\n");
    	printf("<filename.csv> is the csv file (including the input values) under $UPLOAD_HOME in wsi_config.xml;\n");
    	printf("<N> is the total number of cores;\n");
    	printf("<Limit> is the maximum number of considered candidates (if equal to 0, all the candidates are considered).\n");
    	printf("-d represents debug (on/off)");
    	printf("-i represents the maximum number of iterations");
    	printf("Example:\n");
    	printf("./OPT_JR -f=\"Test3.csv\" -n=220 -k=0 -d=Y -s=dagSim -i=10");
    	printf("/n OPT_JR is executed on a file Test3.csv, the total number of cores is 220, all the candidates are considered, the predictor used is dagSim, "
    			"the maximum number of iterations is 10\n");
    	exit(-1);
}

/*
Function to parse argument from command line;
Invoked by optJrParameters constructor
*/

char* parseArg(char* string, char* gap, int type)
{

        switch(type)
        {
          case NUMBER:
          case STRING:
            return string + strlen(gap);
            break;

          case YES_NO:

            if (strstr(string + strlen(gap), "y") || strstr(string + strlen(gap), "Y")) return "1";

            else if (strstr(string + strlen(gap), "n") || strstr(string + strlen(gap), "N")) return "0";

              else Usage();

            break;

          default:

            printf("Fatal Error: parseArg: option not recognized\n");
            exit(-1);
        }

        return NULL;

}
