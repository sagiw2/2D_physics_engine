cmake_minimum_required(VERSION 3.10)
project(2DPlanetaryEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
include_directories(include)
file(GLOB SOURCES src/*.cpp)


add_executable(main ${SOURCES})


target_link_libraries(main sfml-graphics sfml-window sfml-system)
