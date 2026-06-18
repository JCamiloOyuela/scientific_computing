# ============================================================================
# SIMPLE CMAKE - Final Project VSM
# ============================================================================

cmake_minimum_required(VERSION 3.14)

set( TARGET VSM )

project(${TARGET} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set where to search for Eigen
set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../eigen-5.0.1")

# set where to search for {fmt}
set(LOCAL_FMT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../fmt-12.1.0")
set(LOCAL_FMT_BUILD_DIR "${LOCAL_FMT_DIR}/build")

# for link with {fmt}
if(EXISTS "${LOCAL_FMT_BUILD_DIR}/fmt-config.cmake")
  find_package(fmt CONFIG REQUIRED
    PATHS "${LOCAL_FMT_BUILD_DIR}"
    NO_DEFAULT_PATH
  )
else()
  add_subdirectory("${LOCAL_FMT_DIR}" "${CMAKE_BINARY_DIR}/fmt-build")
endif()

# define executable with sources and headers
add_executable(${TARGET}
  src/${TARGET}_main.cpp
  src/${TARGET}.cpp
  include/${TARGET}.hh
)

add_executable(VSM_autodiff
  src/VSM_autodiff_main.cpp
  src/VSM_autodiff.cpp
  src/VSM_RK4.cpp
  src/VSM_small_signal.cpp
  src/VSM.cpp
  include/VSM_autodiff.hh
  include/VSM_RK4.hh
  include/VSM_small_signal.hh
  include/VSM.hh
)

# extra directory to find headers
target_include_directories(${TARGET}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(VSM_autodiff
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIR}
)

# extra library to link
target_link_libraries(${TARGET} PRIVATE fmt::fmt)

target_link_libraries(VSM_autodiff PRIVATE fmt::fmt)

if (UNIX)
  target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ----------------------------------------------------------------------------
# Doxygen documentation
# ----------------------------------------------------------------------------
find_package(Doxygen)

if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_NAME           "${PROJECT_NAME}")
  set(DOXYGEN_PROJECT_BRIEF          "Virtual Synchronous Machine final project")
  set(DOXYGEN_PROJECT_NUMBER         "${PROJECT_VERSION}")
  set(DOXYGEN_OUTPUT_DIRECTORY       "${CMAKE_CURRENT_SOURCE_DIR}/docs")
  set(DOXYGEN_CREATE_SUBDIRS         NO)
  set(DOXYGEN_GENERATE_HTML          YES)
  set(DOXYGEN_GENERATE_LATEX         NO)
  set(DOXYGEN_USE_MATHJAX            YES)
  set(DOXYGEN_HTML_FORMULA_FORMAT    svg)
  set(DOXYGEN_RECURSIVE              YES)
  set(DOXYGEN_EXTRACT_ALL            YES)
  set(DOXYGEN_EXTRACT_PRIVATE        NO)
  set(DOXYGEN_EXTRACT_STATIC         YES)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "")
  set(DOXYGEN_FULL_PATH_NAMES        YES)
  set(DOXYGEN_STRIP_FROM_PATH        "${CMAKE_CURRENT_SOURCE_DIR}")
  set(DOXYGEN_QUIET                  YES)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED   NO)

  doxygen_add_docs(
    docs
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    COMMENT "Generating API documentation with Doxygen"
  )
else()
  message(STATUS "Doxygen not found: target 'docs' will not be available")
endif()

# post build command, copy the executable to ${CMAKE_SOURCE_DIR}/exe
add_custom_command(TARGET ${TARGET} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory    "${CMAKE_SOURCE_DIR}/exe"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${TARGET}>" "${CMAKE_SOURCE_DIR}/exe/$<TARGET_FILE_NAME:${TARGET}>"
  COMMENT "Copying VSM executable to ${CMAKE_SOURCE_DIR}/exe"
)

add_custom_command(TARGET VSM_autodiff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory    "${CMAKE_SOURCE_DIR}/exe"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:VSM_autodiff>" "${CMAKE_SOURCE_DIR}/exe/$<TARGET_FILE_NAME:VSM_autodiff>"
  COMMENT "Copying VSM_autodiff executable to ${CMAKE_SOURCE_DIR}/exe"
)

message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")