
add_executable(
    tancred_wizard
    RobotInterface.h
    RobotInterface.cpp
    MainWindow.h
    MainWindow.cpp
    VideoWidget.h
    VideoWidget.cpp
    Dataset.h
    Dataset.cpp
    resources.qrc
    main.cpp)

target_link_libraries(
    tancred_wizard
    PUBLIC
    tancred_core
    PkgConfig::libavcodec
    Qt5::Widgets)

