cmake_minimum_required(VERSION 4.1.0)
project(gps_lib_playground VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror -Wpedantic)

add_library(gps_lib INTERFACE)
target_include_directories(gps_lib INTERFACE src/include)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE gps_lib)

# >>> JSON setup

find_package(nlohmann_json CONFIG QUIET)

if (nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
else()
    message(WARNING "nlohmann_json not found, JSON support will not be available")
endif()

# <<< JSON setup

# >>> Doxygen setup

find_package(Doxygen)

if (DOXYGEN_FOUND)
    # Define the output directory as the project root's docs folder
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/docs)

    # Input Doxyfile and output Doxyfile in build directory
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Customize Doxyfile with the output directory
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Create the docs directory if it doesn't exist
    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Doxygen documentation in ${DOXYGEN_OUTPUT_DIR}"
        VERBATIM
    )
else()
    message(WARNING "Doxygen not found, documentation target will not be available")
endif()

# <<< Doxygen setup

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
