#include "utility.hh"

/*
 * 		Name:					doubleCompare
 * 		Input parameters:		double a, double b
 * 		Output parameters:		0  if a = b
 * 								-1 if a < b
 * 								1  if a > b
 * 		Description:			Compare two doubles according to a certain precision (epsilon)
 *
 */

int doubleCompare(double a, double b)
{
	  if (((a - epsilon) < b) && ((a + epsilon) > b))
	   {
	    return 0;
	   }
	  else
	   {
	    if (a > b) return 1; else return -1;
	   }
}



/*
 * 		Name:					checkTotalNodes
 * 		Input parameters:		int N, sApplication * pointer
 * 		Output parameters:		TBD
 * 		Description:			It checks that the total allocated nodes is still less or equal than the total number of cores available N
 *


void checkTotalNodes(int N, Batch &app_manager)
{
	int total = 0;

	for (auto it= app_manager.APPs.begin(),  it != app_manager.APPs.begin(), ++it)
	{
		total+= it->currentCores_d;
		if (total > N)
		{
			printf("Fatal Error: checkTotalNodes: Total current nodes (%d) exceeds maximum nodes number (%d)\n", total, N);
			exit(-1);
		}

	}
}
*/
