/*
 * TILFactory.h
 *
 *  Created on: 20 October 2017
 *      Author: tony
 */

#ifndef COLIN_TILFACTORY_H_
#define COLIN_TILFACTORY_H_

#include <string>
#include <list>

#include "TIL.h"

#include "../FakeTILAction.h"

namespace PDDL {

class TILFactory {
public:

	static TILFactory * getInstance();

	PDDL::TIL getTIL(Planner::FakeTILAction aTIL, double aTimestamp,
		std::list<std::pair<std::string, std::string> > constants = std::list<
				std::pair<std::string, std::string> >());
private:
	//Singleton Instance
	static TILFactory * INSTANCE;
	
	//Private constructor
	TILFactory(){};
	//Singleton
	TILFactory(TILFactory const & other) {
	}
	;
	//Singleton
	TILFactory& operator=(TILFactory const&) {
	}
	;
};

}

#endif /* COLIN_TILFACTORY_H_ */
