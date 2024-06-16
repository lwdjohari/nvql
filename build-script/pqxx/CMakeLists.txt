cmake_minimum_required(VERSION 3.10)

include(ProjectCXX)

set(CXX_VER 0)
NV_GET_CXX_VERSION(CXX_VER)

if(NVQL_FEATURE_POSTGRES AND ${CXX_VER} EQUAL 14)
    message(STATUS "NvServ Postgres: Not yet supported for C++14")
elseif(NVQL_FEATURE_POSTGRES AND ${CXX_VER} EQUAL 17)
    message(STATUS "NvServ Postgres: Linking PQXX 7 for C++${CXX_VER}")
    add_subdirectory(../../deps/pqxx-v7 build-pqxx)
    set(PQXX_INCLUDE_DIRS deps/pqxx-v7/include)
elseif(NVQL_FEATURE_POSTGRES AND ${CXX_VER} EQUAL 20)
    message(STATUS "NvServ Postgres: Linking PQXX 7 for C++${CXX_VER}")
endif()


