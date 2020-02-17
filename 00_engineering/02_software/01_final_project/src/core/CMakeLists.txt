
add_library(tancred_core SHARED tmp.cpp)

target_link_libraries(
    tancred_core
    PUBLIC
    tancred_communication
    realsense2::realsense2
    dlib::dlib)

