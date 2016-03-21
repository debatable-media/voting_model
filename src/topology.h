#pragma once

#include <core/vector.h>
#include "random.h"
#include "metric.h"

using namespace core;

template <int n>
struct topology
{
	topology() {}
	virtual ~topology() {}

	typedef double (*metric)(vector<double, n>, vector<double, n>);

	virtual vector<double, n> random(real_distribution random) = 0;
	virtual double similarity(vector<double, n> v0, vector<double, n> v1, metric distance = euclidean<n>) = 0;
};

template <int n>
struct cube : topology<n>
{
	cube()
	{
		pos.fill(0.0);
		radius = 1.0;
	}

	cube(vector<double, n> pos, double radius)
	{
		this->pos = pos;
		this->radius = radius;
	}

	~cube() {}

	using typename topology<n>::metric;

	vector<double, n> pos;
	double radius;

	vector<double, n> random(real_distribution sub)
	{
		vector<double, n> result;
		for (int i = 0; i < n; i++)
			result[i] = sub(pos[i] - radius, pos[i] + radius);
		return result;
	}

	double similarity(vector<double, n> v0, vector<double, n> v1, metric distance = euclidean<n>)
	{
		return (1.0 - distance(v0, v1)/(sqrt((double)n)*2.0*radius));
	}
};

template <int n>
struct ball : topology<n>
{
	ball()
	{
		pos.fill(0.0);
		radius = 1.0;
	}

	ball(vector<double, n> pos, double radius)
	{
		this->pos = pos;
		this->radius = radius;
	}

	~ball() {}

	using typename topology<n>::metric;

	vector<double, n> pos;
	double radius;

	vector<double, n> random(real_distribution sub)
	{
		vector<double, n> result;
		do
		{
			for (int i = 0; i < n; i++)
				result[i] = sub(pos[i]-radius, pos[i]+radius);
		} while (dist(result, pos) > radius);
		return result;
	}

	double similarity(vector<double, n> v0, vector<double, n> v1, metric distance = euclidean<n>)
	{
		return (1.0 - distance(v0, v1)/(2.0*radius));
	}
};


