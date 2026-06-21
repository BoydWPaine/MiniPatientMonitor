add_executable(mini_host
    src/main.cpp
    src/net_client.cpp
)

target_link_libraries(mini_host PRIVATE mpm_common)

set_target_properties(mini_host PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/host
)