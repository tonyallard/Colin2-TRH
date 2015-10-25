cmake_minimum_required(VERSION 2.4)

project(PDDL)

set(CMAKE_CXX_FLAGS "-std=c++0x -pg -ggdb ${CMAKE_CXX_FLAGS}")

include_directories(${val_SOURCE_DIR} ${val_SOURCE_DIR}/parsing)

set(libPDDL_SRCS
	PDDLObject.cpp
	Proposition.cpp
	Literal.cpp
	PNE.cpp	
	TIL.cpp
	PendingProposition.cpp    
	PendingPNE.cpp
	PendingAction.cpp
	MMCRDomainFactory.cpp
	PDDLState.cpp
	PDDLUtils.cpp
	PDDLStateFactory.cpp
)

add_library(PDDL ${libPDDL_SRCS})
SET_TARGET_PROPERTIES(PDDL PROPERTIES COMPILE_FLAGS "-DTOTALORDERSTATES -DPOPF3ANALYSIS")

