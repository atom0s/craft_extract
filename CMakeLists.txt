# Require newer CMAKE version..
cmake_minimum_required(VERSION 3.22)

# Define the project..
project(craft_extract CXX)

# Setup properties..
set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CXX_EXTENSIONS ON)
set_property(GLOBAL PROPERTY LINKER_LANGUAGE CXX)

# Setup the CMAKE modules path..
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shared/cmake)

# Include the local CMAKE modules..
include(cache)
include(platform)
include(settings)
include(warnings)

# Enable position independent libraries..
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Use pthreads if available..
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

# Link the 'library' to the requested C++ options/standard..
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# Link the warnings defined in the warnings.cmake..
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

# Display build information..
message(STATUS "             CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "          CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "          CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "        CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "     CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "           CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "           CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "  CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")

# Setup parallel building if not already set by the user..
if (NOT CMAKE_BUILD_PARALLEL_LEVEL)
    # Obtain the local processor count..
    include(ProcessorCount)
    ProcessorCount(N)

    # Set the count if multiple cores are available..
    if (NOT N EQUAL 0)
        set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
    endif()
endif()
message(STATUS "CMAKE_BUILD_PARALLEL_LEVEL: ${CMAKE_BUILD_PARALLEL_LEVEL}")

# Include ClangTidy..
include(clangtidy)

#
# Parsers Settings
#

set(craft_extract_lib_paths
    "ext/sqlite3/lib/"
)
set(craft_extract_lib
    "sqlite3"
)
set(craft_extract_src
    "src/defines.hpp"
    "src/main.cpp"
    "src/v66.hpp"
    "src/v67.hpp"

    # sqlitecpp
    "ext/sqlitecpp/src/Backup.cpp"
    "ext/sqlitecpp/src/Column.cpp"
    "ext/sqlitecpp/src/Database.cpp"
    "ext/sqlitecpp/src/Exception.cpp"
    "ext/sqlitecpp/src/Savepoint.cpp"
    "ext/sqlitecpp/src/Statement.cpp"
    "ext/sqlitecpp/src/Transaction.cpp"
)
set(craft_extract_inc
    "ext/cxxopts/"
    "ext/nlohmann_json/"
    "ext/sqlite3/include/"
    "ext/sqlitecpp/include/"
)

if (WIN32)
    set(craft_extract_res "${CMAKE_SOURCE_DIR}/res/resources.rc")
endif()

add_executable(craft_extract ${craft_extract_src} ${craft_extract_res})
target_include_directories(craft_extract PUBLIC ${craft_extract_inc})
target_link_directories(craft_extract PUBLIC ${craft_extract_lib_paths})
target_link_libraries(craft_extract PUBLIC ${craft_extract_lib})

if (WIN32)
    set_target_properties(craft_extract PROPERTIES
        OUTPUT_NAME craft_extract
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
endif()
