#include "utility.hh"






double elapsedTime(struct timeval  tv1, struct timeval tv2)
{
	return (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	                     (double) (tv2.tv_sec - tv1.tv_sec);
}





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
