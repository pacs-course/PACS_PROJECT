#ifndef UTILITY_HH
#define UTILITY_HH

#include <sys/time.h>


const double epsilon = 0.001; ///< precision in doubleCompare



/**
Measures the elapsed time
*/
double elapsedTime(struct timeval  tv1, struct timeval tv2);

/**
"doubleCompare" compare two doubles according to a certain precision (epsilon)
*/
int doubleCompare(double a, double b);


#endif
