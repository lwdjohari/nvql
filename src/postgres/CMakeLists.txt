cmake_minimum_required(VERSION 3.10)
project(nvserv CXX)


message(STATUS "NvQL Postgres : Configure")
message(STATUS "-----------------------------------")
message(STATUS "Compile Flags: ${NVQL_FEATURE_DEFINITION}")

# Add dependencies source code
add_subdirectory(../../build-script/pqxx build-deps-pqxx)

#Main headers and sources
file(GLOB_RECURSE SOURCES_NVSERV_PG CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)

# nvserv::core
add_library(${PROJECT_NAME}_postgres STATIC ${SOURCES_NVSERV_PG} )
target_link_libraries(${PROJECT_NAME}_postgres  
    PUBLIC
        ${PROJECT_NAME}::core 
    PRIVATE
        pqxx
    )

set_target_properties(${PROJECT_NAME}_postgres PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_postgres PUBLIC ${CXX_FEATURE})
target_compile_definitions(${PROJECT_NAME}_postgres PUBLIC ${NVQL_FEATURE_DEFINITION})
target_include_directories(${PROJECT_NAME}_postgres
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PRIVATE
        ${PQXX_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}::postgres ALIAS ${PROJECT_NAME}_postgres )