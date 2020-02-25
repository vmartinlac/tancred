
add_executable(
    tancred_wizard
    LCMThread.h
    LCMThread.cpp
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

