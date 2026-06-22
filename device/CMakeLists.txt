add_executable(mini_device
    src/main.cpp
    src/net_server.cpp
    src/device_config.cpp
    src/ecg_sim.cpp
    src/spo2_sim.cpp
    src/resp_sim.cpp
    src/temp_sim.cpp
    src/nibp_sim.cpp
)

target_link_libraries(mini_device PRIVATE mpm_common)

set_target_properties(mini_device PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/device
)