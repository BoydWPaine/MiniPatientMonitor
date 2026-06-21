add_executable(mini_device
    src/main.cpp
    src/net_server.cpp
)

target_link_libraries(mini_device PRIVATE mpm_common)

set_target_properties(mini_device PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/device
)