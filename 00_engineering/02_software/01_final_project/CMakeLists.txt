
cmake_minimum_required(VERSION 3.10)

project(tancred)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
find_package(YARP REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc)
find_package(realsense2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql)
find_package(Threads REQUIRED)
find_package(Torch REQUIRED)

#pkg_check_modules(sqlite3 REQUIRED IMPORTED_TARGET sqlite3)
#pkg_check_modules(libavcodec REQUIRED IMPORTED_TARGET libavcodec)
#pkg_check_modules(tbb REQUIRED IMPORTED_TARGET tbb)

add_subdirectory(app)
add_subdirectory(src)

