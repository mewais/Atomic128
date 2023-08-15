cmake_minimum_required(VERSION 3.10)
project(Atomic128)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-strict-aliasing -pthread -O3")

option(ENABLE_TESTS "Enable tests" OFF)

##################################################################################
################################### Library ######################################
##################################################################################
file(GLOB header_files include/Atomic128/*.hpp)

add_library(Atomic128 INTERFACE)
target_include_directories(Atomic128 INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX})

###################################################################################
###################################### Test #######################################
###################################################################################
if (${ENABLE_TESTS})
    include(CTest)
    enable_testing()
    file(GLOB tests test/*.cpp)
    include_directories(include/)

    foreach(test ${tests})
        string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" basetest ${test})
        add_executable(${basetest} ${test})
        add_test(${basetest} ${basetest})
    endforeach()
endif()
