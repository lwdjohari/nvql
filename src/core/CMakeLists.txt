cmake_minimum_required(VERSION 3.10)
project(nvserv CXX)

if(NVQL_STANDALONE)

message(STATUS "NvQL Core : Configure")
message(STATUS "-----------------------------------")
message(STATUS "Compile Flags: ${NVQL_FEATURE_DEFINITION}")


#Main headers and sources
file(GLOB_RECURSE SOURCES_NVSERV_CORE CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)


# nvserv::core
add_library(${PROJECT_NAME}_core STATIC ${SOURCES_NVSERV_CORE} )
target_link_libraries(${PROJECT_NAME}_core  
    PUBLIC
        nvcore
    )
set_target_properties(${PROJECT_NAME}_core PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_core PUBLIC ${CXX_FEATURE})
target_compile_definitions(${PROJECT_NAME}_core PUBLIC ${NVQL_FEATURE_DEFINITION})
target_include_directories(${PROJECT_NAME}_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
   
)

add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core )

endif()