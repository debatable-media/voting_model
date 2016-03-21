#pragma once

#include "topology.h"
#include "random.h"
#include "metric.h"
#include "population.h"

typedef array<int> ballot;

template <int n>
struct voting_system
{
	voting_system(population<n> *people, int num_seats)
	{
		this->people = people;
		this->num_seats = num_seats;
	}

	virtual ~voting_system() {}

	population<n> *people;
	array<ballot> ballots;
	array<int> ranking;
	int num_seats;

	void drop(int i)
	{
		people->drop(i);
		for (int j = 0; j < ballots.size(); j++)
			ballots[j].at(i).pop();
		for (int j = 0; j < ranking.size(); j++)
			if (ranking[j] == i)
				ranking.at(j).pop();
	}

	static double representation(const person<n> &p, array<int> winners)
	{
		double total = 0.0;
		for (int i = 0; i < winners.size(); i++)
			total += 1.0 - p.preference[winners[i]];
		total /= (double)winners.size();
		return total;
	}

	static double weighted_representation(const person<n> &p, array<int> rank, int num_seats)
	{
		double total = 0.0;
		double weight = 1.0;
		for (int i = 0; i < rank.size(); i++)
		{
			if (i >= num_seats)
				weight *= 0.5;
			total += weight*(1.0 - p.preference[rank[i]]);
		}
		total /= (double)(num_seats + 1);
		return total;
	}

	vector<double, 7> representation()
	{
		if (ranking.size() == 0)
			printf("error: please run the election first.\n");
		else if (ranking.size() < num_seats)
			printf("error: not enough candidates to fill all of the seats.\n");

		// Get all representation values
		array<double> rep;
		rep.reserve(people->electorate.size());
		array<int> winners = ranking.sub(0, num_seats-1);
		for (int i = 0; i < people->electorate.size(); i++)
			rep.push_back(representation(people->electorate[i], winners));

		// Get the Expected Value
		vector<double, 7> result;
		result.fill(0.0);
		for (int i = 0; i < rep.size(); i++)
			result[0] += rep[i];
		result[0] /= (double)rep.size();
		
		// Get the Standard Deviation
		for (int i = 0; i < rep.size(); i++)
			result[1] += (rep[i] - result[0])*(rep[i] - result[0]);
		result[1] = sqrt(result[1]/(double)rep.size());

		// Get the distribution
		for (int i = 0; i < rep.size(); i++)
			result[2+(int)(rep[i]*5.0)]++;

		return result;
	}

	virtual ballot vote(const person<n> &p) = 0;
	virtual ballot revote(const person<n> &p, const ballot &prev, int distance) = 0;
	
	void run_first_election()
	{
		ballots.clear();
		ballots.reserve(people->electorate.size());
		for (int i = 0; i < people->electorate.size(); i++)
			ballots.push_back(vote(people->electorate[i]));
	}

	void run_next_election(int distance, int attempts, int optimism)
	{
		array<int> potential;
		for (int i = 0; i < ballots.size(); i++)
		{
			double rep = weighted_representation(people->electorate[i], ranking, num_seats);
			for (int j = 0; j < attempts; j++)
			{
				ballot next = revote(people->electorate[i], ballots[i], distance);
				potential = re_eval(ballots[i], next, optimism);
				double next_rep = weighted_representation(people->electorate[i], potential, num_seats); 
				if (next_rep > rep)
				{
					rep = next_rep;
					ballots[i] = next;
				}
			}
		}
	}

	virtual array<int> eval() = 0;
	virtual array<int> re_eval(ballot prev, ballot test, int optimism) = 0;
};

template <int n>
struct plurality : voting_system<n>
{
	plurality(population<n> *people, int num_seats, int count) : voting_system<n>(people, num_seats)
	{
		this->count = count;
	}

	~plurality() {}

	int count;
	ballot total;

	void drop(int i)
	{
		total.at(i).pop();
		voting_system<n>::drop(i);
	}

	ballot vote(const person<n> &p)
	{
		ballot result;
		result.reserve(p.preference.size());
		double total = 0.0f;
		for (int i = 0; i < p.preference.size(); i++)
			total += p.preference[i];

		int total_count = 0;
		double closest = -1.0f;
		int closest_index = -1;
		for (int i = 0; i < p.preference.size(); i++)
		{
			result.push_back((int)((double)count*p.preference[i]/total));
			total_count += result[i];
			if (p.preference[i] < closest)
			{
				closest = p.preference[i];
				closest_index = i;
			}
		}

		if (closest < 0)
			result[rand()%result.size()] += count - total_count;
		else
			result[closest_index] += count - total_count;
		
		return result;
	}

	ballot revote(const person<n> &p, const ballot &prev, int distance)
	{
		ballot result = prev;
		array<int> increase;
		array<int> decrease;
		increase.reserve(result.size());
		decrease.reserve(result.size());
		for (int i = 0; i < distance; i++)
		{
			increase.clear();
			decrease.clear();
			for (int j = 0; j < result.size(); j++)
			{
				if (result[j] > 0)
					decrease.push_back(j);
				if (result[j] < count)
					increase.push_back(j);
			}
			
			if (increase.size() > 0 && decrease.size() > 0)
			{
				result[increase[rand()%increase.size()]]++;
				result[decrease[rand()%decrease.size()]]--;	
			}
		}
		return result;
	}

	array<int> eval()
	{
		total.clear();
		total.push_back(this->people->candidates.size(), 0);
		for (int i = 0; i < this->ballots.size(); i++)
			for (int j = 0; j < total.size(); j++)
				total[j] += this->ballots[i][j];

		this->ranking.clear();
		this->ranking.reserve(total.size());
		for (int i = 0; i < total.size(); i++)
			this->ranking.push_back(i);

		order_quick(total, this->ranking);
		reverse(this->ranking);
		printf("{");
		for (int i = 0; i < this->ranking.size(); i++)
		{
			if (i == this->num_seats)
				printf("} {");
			else if (i != 0)
				printf(", ");

			printf("%d %d", this->ranking[i], total[this->ranking[i]]);
		}
		printf("}");

		return this->ranking;
	}

	array<int> re_eval(ballot prev, ballot test, int optimism)
	{
		int missing = 0;
		ballot outcome = test;
		int added = 0;
		for (int i = 0; i < test.size(); i++)
		{
			added += test[i]*optimism;
			outcome[i] = (test[i] - prev[i])*optimism + total[i];
			if (outcome[i] < 0)
			{
				missing -= outcome[i];
				outcome[i] = 0;
			}
		}

		if (added != 0)
			for (int i = 0; i < outcome.size(); i++)
				outcome[i] -= test[i]*optimism*missing/added;
	
		array<int> result;
		result.reserve(test.size());
		for (int i = 0; i < test.size(); i++)
			result.push_back(i);

		order_quick(test, result);
		reverse(result);
		return result;
	}
};

template <int n>
struct approval : voting_system<n>
{
	approval(population<n> *people, int num_seats, int levels) : voting_system<n>(people, num_seats)
	{
		this->levels = levels;
	}

	~approval() {}

	int levels;
	ballot total;

	void drop(int i)
	{
		total.at(i).pop();
		voting_system<n>::drop(i);
	}

	ballot vote(const person<n> &p)
	{
		ballot result;
		result.reserve(p.preference.size());
		for (int i = 0; i < p.preference.size(); i++)
			result.push_back((int)(p.preference[i]*(double)(levels+1)));
		return result;
	}

	ballot revote(const person<n> &p, const ballot &prev, int distance)
	{
		ballot result = prev;
		array<int> increase;
		array<int> decrease;
		increase.reserve(result.size());
		decrease.reserve(result.size());
		for (int i = 0; i < distance; i++)
		{
			increase.clear();
			decrease.clear();
			for (int j = 0; j < result.size(); j++)
			{
				if (result[j] > 0)
					decrease.push_back(j);
				if (result[j] < levels)
					increase.push_back(j);
			}

			int direction = rand()%2;
			if (direction == 0 && decrease.size() > 0)
				result[decrease[rand()%decrease.size()]]--;
			else if (increase.size() > 0)
				result[increase[rand()%increase.size()]]++;
		}
		return result;
	}

	array<int> eval()
	{
		total.clear();
		total.push_back(this->people->candidates.size(), 0);
		for (int i = 0; i < this->ballots.size(); i++)
			for (int j = 0; j < total.size(); j++)
				total[j] += this->ballots[i][j];
		
		this->ranking.clear();
		this->ranking.reserve(total.size());
		for (int i = 0; i < total.size(); i++)
			this->ranking.push_back(i);

		order_quick(total, this->ranking);
		reverse(this->ranking);
		printf("{");
		for (int i = 0; i < this->ranking.size(); i++)
		{
			if (i == this->num_seats)
				printf("} {");
			else if (i != 0)
				printf(", ");

			printf("%d %d", this->ranking[i], total[this->ranking[i]]);
		}
		printf("}");

		return this->ranking;
	}

	array<int> re_eval(ballot prev, ballot test, int optimism)
	{
		for (int i = 0; i < test.size(); i++)
			test[i] = max(0, (test[i] - prev[i])*optimism + total[i]);
		
		array<int> result;
		result.reserve(test.size());
		for (int i = 0; i < test.size(); i++)
			result.push_back(i);

		order_quick(test, result);
		reverse(result);
		return result;
	}
};

