#include <core/array.h>
#include <core/vector.h>
#include <time.h>

#include "random.h"
#include "topology.h"
#include "metric.h"
#include "vote.h"

using namespace core;

int main()
{
	srand(time(0));
	cube<3> geometry;
	population<3> people(&geometry);
	plurality<3> system(&people, 1, 1);
	
	people.generate(100000, 10, &uniform);
	system.run_first_election();
	system.eval();
	cout << "\t" << system.representation() << endl;
	for (int i = 0; i < 20; i++)
	{
		system.run_next_election(1, 10, 1000);
		system.eval();
		cout << "\t" << system.representation() << endl;

		if (system.ranking.size() > 2)
			system.drop(system.ranking.back());
	}

	return 0;
}

