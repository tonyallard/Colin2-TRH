//#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>

#include "TRH.h"

namespace TRH {

TRH * TRH::INSTANCE = NULL;
const char * TRH::H_CMD = "./lib/colin-clp tempdomain.pddl temp.pddl";
const string TRH::H_VAL_DELIM = "State Heuristic Value is: ";

TRH * TRH::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new TRH();
	}
	return INSTANCE;
}

double TRH::getHeuristic(PDDL::PDDLState state) {
	string filePath = "";
	string fileName = "temp";
	state.writeDeTILedStateToFile(filePath, fileName);
	state.writeDeTILedDomainToFile(filePath, fileName);

	std::shared_ptr<FILE> pipe(popen(H_CMD, "r"), pclose);
	if (!pipe)
		return 1e5;
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	int pos = result.find(H_VAL_DELIM);
	string h_val_str = result.substr(pos + H_VAL_DELIM.size());
	double hval = stod(h_val_str);
	return hval;
}
}
