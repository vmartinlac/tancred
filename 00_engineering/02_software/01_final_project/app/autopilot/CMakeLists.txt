
add_executable(tancred_autopilot main.cpp)

target_link_libraries(tancred_autopilot PUBLIC ${TORCH_LIBRARIES} opencv_core opencv_imgcodecs tancred_core)
target_compile_definitions(tancred_autopilot PUBLIC _GLIBCXX_USE_CXX11_ABI=1)

