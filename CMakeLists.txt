cmake_minimum_required(VERSION 3.10)
project(NvQL CXX)


message(STATUS "NvQL : Configure")
message(STATUS "-----------------------")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(NVQL_LIB_NAME "nvserv")

if(NOT DEFINED NVSERV_LIBRARY_VERSION OR NOT DEFINED NVSERV_LIBRARY_SOVERSION)
    message(FATAL_ERROR "nvql configuration ERROR:
      please set option NVSERV_LIBRARY_VERSION or NVSERV_LIBRARY_SOVERSION!")
endif()

if(NOT NVSERV_BUILD_LOCAL_SHARED AND NOT NVSERV_BUILD_STATIC)
    message(FATAL_ERROR "nvql configuration ERROR:
      please set option NVSERV_BUILD_STATIC or NVSERV_BUILD_LOCAL_SHARED!")
endif()

set(_LIBRARY_TYPE SHARED STATIC)
set(NVQL_FEATURE_DEFINITION)

if(NVSERV_BUILD_LOCAL_SHARED)
    set(_LIBRARY_TYPE SHARED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    LIST(APPEND NVQL_FEATURE_DEFINITION NVSERV_BUILD_LOCAL_SHARED=1)
endif()

# Storage feature
option(NVQL_FEATURE_POSTGRES "Use NVQL postgres datalayer" ON)
option(NVQL_STANDALONE "Use NVQL Standalone separate from nvserv" ON)

include(ProjectCXX)
set(ISROOT FALSE)
set(CXX_FEATURE "")
NV_DETECT_IS_ROOT_PROJECT(ISROOT)

message(STATUS "NvServ Root Project: ${ISROOT} ${NVSERV_CXX_VERSION}")

if(ISROOT)
    set(NVQL_STANDALONE ON)
    set(NVQL_FEATURE_POSTGRES ON)
    if(NOT NVSERV_CXX_VERSION)
    set(NVSERV_CXX_VERSION 17)
    endif()
    option(NVSERV_CXX_STANDARD_REQUIRED "CXX Required" ON)
    option(NVSERV_CXX_EXTENSIONS "CXX Extensions" ON)
    NV_PROPOGATE_CXX_OPTIONS(${NVSERV_CXX_VERSION} ${NVSERV_CXX_STANDARD_REQUIRED} ${NVSERV_CXX_EXTENSIONS})
else()
    set(NVQL_STANDALONE OFF)
endif()

NV_GET_CXX_STD_FEATURE(${NVSERV_CXX_VERSION} CXX_FEATURE)
message(STATUS "CXX Feature: ${CXX_FEATURE}")


if(NVQL_STANDALONE)
    #nvserv-core
    LIST(APPEND NVQL_FEATURE_DEFINITION NVQL_STANDALONE=1)
else()
    LIST(APPEND NVQL_FEATURE_DEFINITION NVQL_STANDALONE=0)
endif()

# Convert the list to a comma-separated string
string(JOIN ", " NVQL_FEATURES_JOIN ${NVQL_FEATURE_DEFINITION})

# Print the comma-separated list
message(STATUS "Compile Flags: ${NVQL_FEATURES_JOIN}")

# nvql::postgres

if(NOT nvcore_FOUND)
    message(STATUS "Nvql lib nvcore: Initialize NvCore lib.")
    add_subdirectory(deps/nvcore/ build-nvcore)
else()
    message(STATUS "Nvql lib nvcore: nvcore lib are already set, use nvcore from project.")
endif()



if (TARGET absl::base)
    message(STATUS "Nvql lib ABSL: Abseil lib are already set, use ABSL from project.")
else ()
    message(STATUS "Nvql lib ABSL: initialize Abseil lib.")
    set(ABSL_PROPAGATE_CXX_STD ON)
    add_subdirectory(deps/nvserv-core/deps/abseil-lts20230125.1 build-abseil)
endif ()

if (TARGET nvserv::core)
    message(STATUS "Nvserv::core: nvserv::core are already set, use nvserv from project.")
else()
    message(STATUS "Nvserv::core: initialize nvserv::core.")
    add_subdirectory(deps/nvserv-core/ build-nvserv_core)
endif()

add_subdirectory(src/storage/ build-nvserv_storage)

if(NVQL_FEATURE_POSTGRES)
  add_subdirectory(src/postgres/ build-nvserv_postgres)
endif()




add_executable(${PROJECT_NAME}_runner src/main.cc)
target_link_libraries(${PROJECT_NAME}_runner PUBLIC nvserv::core nvserv::postgres)
set_target_properties(${PROJECT_NAME}_runner PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_runner PUBLIC ${CXX_FEATURE})
# target_include_directories(${PROJECT_NAME}_runner
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src/
# )
