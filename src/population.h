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

	// Each person's political views are modelled as a point
	// in an N-dimensional space of some topology. 
	vector<double, n> view;

	// Their preferences are numbers from 0.0 (strong dislike) 
	// to 1.0 (strong like) for each candidate.
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

	// A population contains a bunch of people
	array<person<n> > electorate;

	// distributed within some N-dimensional topology
	topology<n> *geometry;

	// some of whom are candidates up for election
	array<int> candidates;

	// Randomly generate the voter_count people in the electorate with candidate_count candidates
	// using the random distribution specified.
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

	// Drop a candidate from the election. Candidate remains in electorate.
	void drop(int i)
	{
		candidates.at(i).pop();
		for (int i = 0; i < electorate.size(); i++)
			electorate[i].preference.at(i).pop();
	}

	// Thermo-nuclear warefare. Everyone dies.
	void clear()
	{
		electorate.clear();
		candidates.clear();
	}
};

