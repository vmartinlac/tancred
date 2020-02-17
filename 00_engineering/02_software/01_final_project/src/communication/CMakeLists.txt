
lcm_wrap_types(
    C_EXPORT tancred
    C_SOURCES COMM_C_SOURCES
    C_HEADERS COMM_C_HEADERS
    CPP_HEADERS COMM_CPP_HEADERS
    types.lcm)

lcm_add_library(tancred_communication CPP ${COMM_CPP_HEADERS})

target_include_directories(tancred_communication INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

