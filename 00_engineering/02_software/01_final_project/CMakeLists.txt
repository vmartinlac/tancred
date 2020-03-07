
cmake_minimum_required(VERSION 3.10)

project(tancred)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(YARP REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc)
find_package(realsense2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql)

pkg_check_modules(sqlite3 REQUIRED IMPORTED_TARGET sqlite3)

add_subdirectory(app)
add_subdirectory(src)

