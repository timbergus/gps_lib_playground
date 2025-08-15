cmake_minimum_required(VERSION 4.1.0)
project(gps_lib_playground VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(AUTHOR "Gustavo Muñoz")
set(DESCRIPTION "A header-only C++ library for parsing NMEA GPS sentences")

add_compile_options(-Wall -Wextra -Werror -Wpedantic)

include(FetchContent)

# >>> JSON download
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)
# <<< JSON download

# >>> Include gps_lib
add_library(gps_lib INTERFACE)
target_include_directories(gps_lib INTERFACE src/include)
# <<< Include gps_lib

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
  gps_lib
  nlohmann_json::nlohmann_json
)

# >>> Doxygen setup
find_package(Doxygen)

if (DOXYGEN_FOUND)
  # Define the output directory as the project root's docs folder
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/docs)
  set(DOXYGEN_INPUT_DIR ${CMAKE_SOURCE_DIR}/src)

  # Input Doxyfile and output Doxyfile in build directory
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # Customize Doxyfile with the output directory
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Doxygen documentation in ${DOXYGEN_OUTPUT_DIR}"
    VERBATIM
  )

  install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html/
    DESTINATION docs
    COMPONENT docs
  )

  install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/
    DESTINATION bin/data
    COMPONENT runtime
  )
else()
  message(WARNING "Doxygen not found, documentation target will not be available")
endif()
# <<< Doxygen setup

include(CTest)
enable_testing()

# >>> Packaging the project for installation
include(InstallRequiredSystemLibraries)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPS Library Playground")
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Gustavo Muñoz")
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PROJECT_NAME}-${CPACK_PROJECT_VERSION}")
include(CPack)
# <<< Packaging the project for installation
