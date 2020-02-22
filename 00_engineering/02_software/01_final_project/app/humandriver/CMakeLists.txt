
add_executable(
    tancred_humandriver
    MainWindow.h
    MainWindow.cpp
    VideoWidget.h
    VideoWidget.cpp
    ConfigDialog.h
    ConfigDialog.cpp
    ConfigDialog.ui
    Dataset.h
    Dataset.cpp
    main.cpp)

target_link_libraries(
    tancred_humandriver
    PUBLIC
    tancred_core
    PkgConfig::sqlite3
    Qt5::Widgets)

