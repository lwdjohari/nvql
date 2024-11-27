cmake_minimum_required(VERSION 3.10)
project(nvserv CXX)

message(STATUS "NvQL Storage Core : Configure")
message(STATUS "-----------------------------------")
message(STATUS "Compile Flags: ${NVQL_FEATURE_DEFINITION}")


#Main headers and sources
file(GLOB_RECURSE SOURCES_NVSERV_STORAGE CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)


# nvserv::core
add_library(${PROJECT_NAME}_storage STATIC ${SOURCES_NVSERV_STORAGE} )
target_link_libraries(${PROJECT_NAME}_storage  
    PUBLIC
        nvcore
        nvserv::core
        absl::base 
        absl::base 
        absl::flat_hash_map 
        absl::algorithm
        absl::numeric
        absl::random_random
        absl::synchronization 
        absl::strings
        absl::time
        absl::utility
        absl::status
    )
set_target_properties(${PROJECT_NAME}_storage PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_storage PUBLIC ${CXX_FEATURE})
target_compile_definitions(${PROJECT_NAME}_storage PUBLIC ${NVQL_FEATURE_DEFINITION})
target_include_directories(${PROJECT_NAME}_storage
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
   
)

add_library(${PROJECT_NAME}::storage ALIAS ${PROJECT_NAME}_storage )