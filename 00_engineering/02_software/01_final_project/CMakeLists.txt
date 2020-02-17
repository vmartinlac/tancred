
cmake_minimum_required(VERSION 3.0)

project(tancred)

find_package(lcm REQUIRED)
find_package(dlib REQUIRED)
find_package(realsense2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

include(${LCM_USE_FILE})

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tst)

