include(FetchContent)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v21.12
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(protobuf)

set(MONITOR_PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/monitor.proto)
set(MONITOR_PROTO_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${MONITOR_PROTO_OUT_DIR})

set(MONITOR_PROTO_SRCS
    ${MONITOR_PROTO_OUT_DIR}/monitor.pb.cc
)
set(MONITOR_PROTO_HDRS
    ${MONITOR_PROTO_OUT_DIR}/monitor.pb.h
)

add_custom_command(
    OUTPUT ${MONITOR_PROTO_SRCS} ${MONITOR_PROTO_HDRS}
    COMMAND protobuf::protoc
        --cpp_out=${MONITOR_PROTO_OUT_DIR}
        -I${CMAKE_CURRENT_SOURCE_DIR}/proto
        ${MONITOR_PROTO_FILE}
    DEPENDS ${MONITOR_PROTO_FILE} protobuf::protoc
    COMMENT "Generating monitor.pb.cc/h from monitor.proto"
    VERBATIM
)

add_library(mpm_common STATIC
    ${MONITOR_PROTO_SRCS}
    proto/encoder.cpp
    osal/port_common.cpp
    $<$<BOOL:${WIN32}>:osal/port_windows.cpp>
    $<$<NOT:$<BOOL:${WIN32}>>:osal/port_linux.cpp>
)

target_include_directories(mpm_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${MONITOR_PROTO_OUT_DIR}
)

target_link_libraries(mpm_common
    PUBLIC
        protobuf::libprotobuf
)

if(WIN32)
    target_link_libraries(mpm_common PUBLIC ws2_32)
endif()

target_compile_definitions(mpm_common
    PUBLIC
        MPM_TCP_PORT=${MPM_TCP_PORT}
        MPM_TCP_HOST="${MPM_TCP_HOST}"
)