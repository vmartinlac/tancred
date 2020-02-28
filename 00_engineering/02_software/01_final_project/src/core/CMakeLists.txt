
add_library(
    tancred_core
    SHARED
    common.h
    tmp.cpp)

target_link_libraries(
    tancred_core
    PUBLIC
    tancred_communication
    dlib::dlib)

target_include_directories(
    tancred_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

