#include <iostream>
#include <fstream>

#include "include/json.hpp"

#include "simulation.h"

using json = nlohmann::json;

json networkModel;

int main()
{
	std::cout << "Start\n";

	std::ifstream fstream("./data/network_model.json");
	fstream >> networkModel;
	fstream.close();

	Simulation *simulation = new Simulation(networkModel);
	simulation->run();

	std::cout << "\n Koniec";

	return 0;
}
