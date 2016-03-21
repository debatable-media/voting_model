#pragma once

#include "topology.h"
#include "random.h"
#include "metric.h"

template <int n>
struct person
{
	person() {}
	person(vector<double, n> view)
	{
		this->view = view;
	}
	~person() {}

	vector<double, n> view;
	array<double> preference;
};

template <int n>
struct population
{
	population(topology<n> *geometry)
	{
		this->geometry = geometry;
	}

	~population()
	{
	}

	array<person<n> > electorate;
	array<int> candidates;
	topology<n> *geometry;

	void generate(int voter_count, int candidate_count, real_distribution distribution)
	{
		//printf("Generating population of %d.\n", voter_count);
		electorate.reserve(voter_count);
		for (int i = 0; i < voter_count; i++)
			electorate.push_back(person<n>(geometry->random(distribution)));

		//printf("Picking %d candidates.\n", candidate_count);
		array<int> temp;
		temp.reserve(voter_count);
		for (int i = 0; i < voter_count; i++)
			temp.push_back(i);

		candidates.reserve(candidate_count);
		for (int i = 0; i < candidate_count && temp.size() > 0; i++)
		{
			int c = rand()%temp.size();
			candidates.push_back(temp[c]);
			temp.at(c).drop();
		}

		//printf("Determining voter preference.\n");
		for (int i = 0; i < electorate.size(); i++)
		{
			electorate[i].preference.reserve(candidate_count);
			for (int j = 0; j < candidate_count; j++)
				electorate[i].preference.push_back(geometry->similarity(electorate[i].view, electorate[candidates[j]].view));
		}
	}

	void drop(int i)
	{
		candidates.at(i).pop();
		for (int i = 0; i < electorate.size(); i++)
			electorate[i].preference.at(i).pop();
	}

	void clear()
	{
		electorate.clear();
		candidates.clear();
	}
};

