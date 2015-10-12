cmake_minimum_required(VERSION 2.4)

project(PDDL)

set(CMAKE_CXX_FLAGS "-std=c++0x -pg -ggdb ${CMAKE_CXX_FLAGS}")

include_directories(${val_SOURCE_DIR} ${val_SOURCE_DIR}/parsing ${val_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(libPDDL_SRCS
	Proposition.cpp
	Literal.cpp
	PNE.cpp	
	TIL.cpp
	PendingProposition.cpp	
	PendingLiteral.cpp    
	PendingPNE.cpp
	PendingAction.cpp
	MMCRDomainFactory.cpp
	PDDLState.cpp
	PDDLUtils.cpp
)

add_library(PDDL STATIC ${libPDDL_SRCS})

