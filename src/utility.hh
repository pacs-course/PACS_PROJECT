#ifndef UTILITY_HH
#define UTILITY_HH

#include <sys/time.h>


const double epsilon = 0.001; ///< precision in double_compare



/**
elapsed_time measures the elapsed time
*/
double elapsed_time(struct timeval  tv1, struct timeval tv2);

/**
double_compare compares two doubles according to a certain precision (epsilon)
*/
int double_compare(double a, double b);


#endif
