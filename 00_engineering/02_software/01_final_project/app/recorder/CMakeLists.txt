
add_executable(
    tancred_recorder
    Constants.h
    Handler.cpp
    Handler.h
    InternalMessages.h
    main.cpp)

target_link_libraries(
    tancred_recorder
    PUBLIC
    tancred_core
    Qt5::Core
    Qt5::Sql)

