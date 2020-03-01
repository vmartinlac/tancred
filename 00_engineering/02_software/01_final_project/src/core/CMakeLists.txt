
add_library(
    tancred_core
    SHARED
    MotorsMessage.h
    MotorsMessage.cpp
    ImageMessage.h
    ImageMessage.cpp)

target_link_libraries(
    tancred_core
    PUBLIC
    YARP::YARP_os
    YARP::YARP_init
    dlib::dlib)

target_include_directories(
    tancred_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

