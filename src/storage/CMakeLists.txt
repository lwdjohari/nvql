cmake_minimum_required(VERSION 3.10)
project(nvserv CXX)

message(STATUS "NvQL Storage Core : Configure")
message(STATUS "-----------------------------------")
message(STATUS "Compile Flags: ${NVQL_FEATURE_DEFINITION}")

if(NVSERV_BUILD_LOCAL_SHARED)
    set(_LIBRARY_TYPE SHARED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

#Main headers and sources
file(GLOB_RECURSE SOURCES_NVSERV_STORAGE CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)


# nvserv::core
add_library(${PROJECT_NAME}_storage ${_LIBRARY_TYPE} ${SOURCES_NVSERV_STORAGE} )
target_link_libraries(${PROJECT_NAME}_storage  
    PUBLIC
        nvcore
        nvserv::core
        # absl::base 
        # absl::base 
        # absl::flat_hash_map 
        # absl::algorithm
        # absl::numeric
        # absl::random_random
        # absl::synchronization 
        # absl::strings
        # absl::time
        # absl::utility
        # absl::status
    )

# Set runtime path for the shared library
if(NVSERV_BUILD_LOCAL_SHARED)
    set_target_properties(${PROJECT_NAME}_storage PROPERTIES
        LINKER_LANGUAGE CXX
        # BUILD_RPATH "$ORIGIN"                   # Use the directory of the binary
        # INSTALL_RPATH "$ORIGIN"                 # Set for installed binaries too
        # BUILD_WITH_INSTALL_RPATH TRUE           # Ensure RPATH is embedded during the build
        VERSION ${NVQL_LIBRARY_VERSION}       # Semantic Version of library
        SOVERSION ${NVQL_LIBRARY_SOVERSION}   # Linker version for sysmlink
    )
endif()

if(NVSERV_BUILD_STATIC)
    set_target_properties(${PROJECT_NAME}_storage PROPERTIES
        LINKER_LANGUAGE CXX
    )
endif()

target_compile_features(${PROJECT_NAME}_storage PUBLIC ${CXX_FEATURE})
target_compile_definitions(${PROJECT_NAME}_storage PUBLIC ${NVQL_FEATURE_DEFINITION})
target_include_directories(${PROJECT_NAME}_storage
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
   
)

add_library(${PROJECT_NAME}::storage ALIAS ${PROJECT_NAME}_storage )