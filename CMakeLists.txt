cmake_minimum_required(VERSION 3.5)
project(jun_qmsInfer)

set(CMAKE_CXX_STANDARD 11)
set(CXX_LINK_FLAGS "-lz3")


add_definitions(${CXX_LINK_FLAGS})
set (CMAKE_CXXFLAGS "-lz3")

INCLUDE_DIRECTORIES(/usr/include)
LINK_DIRECTORIES(/usr/bin/z3 )
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall " )
add_executable(qmsInfer
        "ScInferApproach .h"
        DistributionEnums.h
        DistributionInference.h
        EnumUtil.h
        FileUtil.h
        Node.h
        NodeTypeEnums.h
        NodeUtil.h
        OperatorEnums.h
        SemanticDistributionInference.h
        SetUtil.h
        SMT2Parse.h
        SMTApproach.h
        StringUtil.h
        SymbolicApproach.h
        main.cpp
        QmsStatisfy.h
        QMSApproach.h)
TARGET_LINK_LIBRARIES(qmsInfer z3 -lpthread)
