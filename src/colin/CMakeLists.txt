PROJECT(colin2)

set( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
                          ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
                          ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules)


set(DOXYFILE_TARGET_SUFFIX colin)
include(UseDoxygen OPTIONAL)
include_directories ("${PROJECT_SOURCE_DIR}/pddl")
include_directories ("${PROJECT_SOURCE_DIR}/TRH")

add_subdirectory(util)
add_subdirectory(pddl)
add_subdirectory(kk)
add_subdirectory(stn)
add_subdirectory(itc)
add_subdirectory(executor)
add_subdirectory(hRelax)
add_subdirectory(TRH)

find_package(Clp)
find_package(CoinUtils)
find_package(Cbc)
find_package(Cgl)
find_package(OSI)


set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if(STATIC_COLIN)
set(CMAKE_CXX_LINK_FLAGS "-static ${CMAKE_CXX_LINK_FLAGS}")
endif(STATIC_COLIN)

include_directories(${val_SOURCE_DIR} ${val_SOURCE_DIR}/parsing ${val_BINARY_DIR} ${CLP_INCLUDES}/coin ${CBC_INCLUDES}/coin ${CPLEX_INCLUDES} ${CONCERT_INCLUDES}  ${GSL_INCLUDES})

set(colin_build_srcs
    compressionsafescheduler.cpp
    colintotalordertransformer.cpp
	HTrio.cpp
    minimalstate.cpp
	ExtendedMinimalState.cpp
    FFSolver.cpp
    FFEvent.cpp
    globals.cpp
    lpscheduler.cpp
    RPGBuilder.cpp
    RPGBuilderAnalysis.cpp
    RPGBuilderEvaluation.cpp
    RPGBuilderNumerics.cpp
    temporalanalysis.cpp
    temporalconstraints.cpp
    numericanalysis.cpp
    solver.cpp)

add_library(ColinCommon STATIC ${colin_build_srcs})
target_link_libraries(ColinCommon ParsePDDL Inst UTIL PDDL KK STN EXECUTOR ITC HRELAX TRH)
SET_TARGET_PROPERTIES(ColinCommon PROPERTIES COMPILE_FLAGS "-DTOTALORDERSTATES -DPOPF3ANALYSIS")


if (CLP_INCLUDES AND CBC_INCLUDES)

    ADD_EXECUTABLE(colin-clp colinMain.cpp solver-clp.cpp)

    target_link_libraries(colin-clp
        ColinCommon
        ${CBC_SOLVER_LIBRARY} ${CBC_LIBRARY} ${CGL_LIBRARIES} ${OSICLP_LIBRARIES} ${OSI_LIBRARIES} ${CLP_LIBRARIES}  ${COINUTILS_LIBRARIES} bz2 z
    )
    SET_TARGET_PROPERTIES(colin-clp PROPERTIES COMPILE_FLAGS "-DTOTALORDERSTATES -DPOPF3ANALYSIS")

endif(CLP_INCLUDES AND CBC_INCLUDES)

if(CPLEX AND ILOCPLEX AND CONCERT AND CPLEX_INCLUDES AND CONCERT_INCLUDES)
    ADD_EXECUTABLE(colin-cplex colinMain.cpp solver-cpx.cpp)
    
    target_link_libraries(colin-cplex
        ColinCommon
        ${ILOCPLEX} ${CPLEX} ${CONCERT} pthread bz2 z
    )

    SET_TARGET_PROPERTIES(colin-cplex PROPERTIES COMPILE_FLAGS "-DTOTALORDERSTATES -DPOPF3ANALYSIS")

endif(CPLEX AND ILOCPLEX AND CONCERT AND CPLEX_INCLUDES AND CONCERT_INCLUDES)
