
add_executable(
    tancred_recorder
    main.cpp
    Handler.cpp
    Handler.h
    Constants.h
    InternalMessages.h)

target_link_libraries(
    tancred_recorder
    PUBLIC
    tancred_core
    opencv_core
    opencv_imgcodecs
    PkgConfig::sqlite3
    Qt5::Core)

