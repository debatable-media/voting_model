#pragma once

typedef double (*real_distribution)(double, double);
typedef int (*int_distribution)(int, int);

double uniform(double min, double max);
int uniform(int min, int max);
