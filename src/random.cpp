#include "random.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>

double uniform(double min, double max)
{
	return ((double)rand()/(double)(RAND_MAX-1))*(max - min) + min;
}

int uniform(int min, int max)
{
	return rand()%(max - min + 1) + min;
}

