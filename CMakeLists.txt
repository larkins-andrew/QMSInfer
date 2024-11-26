cmake_minimum_required(VERSION 3.30)
include(FetchContent)
project(qmsInfer)

FetchContent_Declare(z3
        GIT_REPOSITORY https://github.com/Z3Prover/z3
        GIT_TAG        z3-4.13.3
)
FetchContent_MakeAvailable(z3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall " )

include_directories(${qmsInfer_SOURCE_DIR}/include)
include_directories(${z3_SOURCE_DIR}/src/api/c++ )
add_executable(qmsInfer
        src/main.cpp
        src/ScInferApproach.cpp
        src/DistributionInference.cpp
        src/EnumUtil.cpp
        src/FileUtil.cpp
        src/Node.cpp
        src/NodeUtil.cpp
        src/SemanticDistributionInference.cpp
        src/SetUtil.cpp
        src/SMT2Parse.cpp
        src/SMTApproach.cpp
        src/StringUtil.cpp
        src/SymbolicApproach.cpp
        src/QmsStatisfy.cpp
        src/QMSApproach.cpp
)
target_link_libraries(qmsInfer libz3)