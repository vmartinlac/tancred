
add_executable(
    tancred_recorder
    main.cpp)

target_link_libraries(
    tancred_recorder
    PUBLIC
    tancred_core
    opencv_core
    opencv_imgcodecs
    #PkgConfig::sqlite3
    #PkgConfig::tbb
    Threads::Threads)

