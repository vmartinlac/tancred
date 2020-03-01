
add_executable(
    tancred_wizard
    ControllerWidget.h
    ControllerWidget.cpp
    LCMInterface.h
    LCMInterface.cpp
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
    PkgConfig::sqlite3
    Qt5::Widgets)

