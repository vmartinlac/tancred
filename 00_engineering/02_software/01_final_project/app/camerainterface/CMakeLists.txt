
add_executable(
    tancred_camerainterface
    main.cpp)

target_link_libraries(
    tancred_camerainterface
    PUBLIC
    tancred_core
    opencv_imgproc
    Threads::Threads
    #PkgConfig::libavcodec
    realsense2::realsense2)

