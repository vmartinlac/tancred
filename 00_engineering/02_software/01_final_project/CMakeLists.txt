
cmake_minimum_required(VERSION 3.0)

project(tancred)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(YARP REQUIRED)
find_package(dlib REQUIRED)
find_package(realsense2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql)

add_subdirectory(app)
add_subdirectory(src)

