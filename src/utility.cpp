#include "utility.hh"


/**
elapsed_time measures the elapsed time
*/
double elapsed_time(struct timeval  tv1, struct timeval tv2)
{
	return (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	                     (double) (tv2.tv_sec - tv1.tv_sec);
}



/**
double_compare compares two doubles according to a certain precision (epsilon)
*/
int double_compare(double a, double b)
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
