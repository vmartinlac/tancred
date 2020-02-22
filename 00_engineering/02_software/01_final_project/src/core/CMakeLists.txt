
add_library(tancred_core SHARED tmp.cpp)

target_link_libraries(
    tancred_core
    PUBLIC
    tancred_communication
    dlib::dlib)

