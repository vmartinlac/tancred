
add_executable(tmp main.cpp)
target_link_libraries(tmp PUBLIC realsense2::realsense2 dlib::dlib-shared)
message(${dlib_LIBRARIES})

