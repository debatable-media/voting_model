#pragma once

#include <core/vector.h>

using namespace core;

template <int n>
double euclidean(vector<double, n> v0, vector<double, n> v1)
{
	double result = 0.0f;
	for (int i = 0; i < n; i++)
		result += (v1[i] - v0[i])*(v1[i] - v0[i]);
	return sqrt(result);
}


