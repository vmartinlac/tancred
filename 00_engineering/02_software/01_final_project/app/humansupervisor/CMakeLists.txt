
add_executable(
    tancred_humansupervisor
    main.cpp)

target_link_libraries(
    tancred_humansupervisor
    PUBLIC
    tancred_core
    Qt5::Widgets)

