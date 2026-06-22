set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(mini_host
    src/main.cpp
    src/net_thread.cpp
    src/nibp_controller.cpp
    src/ui/main_window.cpp
    src/ui/waveform_widget.cpp
    src/ui/param_panel.cpp
)

target_include_directories(mini_host
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(mini_host
    PRIVATE
        mpm_common
        Qt6::Widgets
)

set_target_properties(mini_host PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/host
    WIN32_EXECUTABLE FALSE
)

option(MPM_BUILD_PACKET_COUNTER "Build packet-rate verification utility" ON)
if(MPM_BUILD_PACKET_COUNTER)
    add_executable(mpm_packet_counter src/packet_counter.cpp)
    target_link_libraries(mpm_packet_counter PRIVATE mpm_common)
    set_target_properties(mpm_packet_counter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/host
    )

    add_executable(mpm_nibp_test src/nibp_test.cpp)
    target_link_libraries(mpm_nibp_test PRIVATE mpm_common)
    set_target_properties(mpm_nibp_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/host
    )
endif()