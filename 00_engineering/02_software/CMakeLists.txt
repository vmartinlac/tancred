
cmake_minimum_required(VERSION 3.0)

project(tancred)

find_package(dlib REQUIRED)
find_package(realsense2 REQUIRED)
find_package(YARP REQUIRED)

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tst)
