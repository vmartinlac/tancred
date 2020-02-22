
cmake_minimum_required(VERSION 3.0)

project(tancred)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(lcm REQUIRED)
find_package(dlib REQUIRED)
find_package(realsense2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

pkg_check_modules(sqlite3 REQUIRED IMPORTED_TARGET sqlite3)

include(${LCM_USE_FILE})

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tst)

