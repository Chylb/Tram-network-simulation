#include <iostream>
#include <fstream>

#include "include/json.hpp"

#include "simulation.h"

using json = nlohmann::json;

json networkModel;

int main()
{
	std::ifstream fstream("./data/network_model.json");
	fstream >> networkModel;
	fstream.close();

	Simulation *simulation = new Simulation(networkModel);
	simulation->run();

	std::ofstream ofstream("./data/results.json");
	ofstream << simulation->getResults();
	ofstream.close();

	return 0;
}
