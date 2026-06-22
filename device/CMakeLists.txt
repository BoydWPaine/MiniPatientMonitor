option(MPM_DEVICE_UI "Build Device LVGL config UI" ON)

set(MPM_LVGL_TAG "v9.2.2" CACHE STRING "LVGL git tag")
set(MPM_SDL2_TAG "release-2.30.8" CACHE STRING "SDL2 git tag")

if(MPM_DEVICE_UI)
    include(FetchContent)

    find_package(SDL2 QUIET)
    if(NOT SDL2_FOUND)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG ${MPM_SDL2_TAG}
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(SDL2)
        set(MPM_SDL_TARGET SDL2::SDL2-static)
    else()
        set(MPM_SDL_TARGET SDL2::SDL2)
    endif()

    set(LV_CONF_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/lv_conf.h
        CACHE STRING "LVGL config header path" FORCE)
    set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        lvgl
        GIT_REPOSITORY https://github.com/lvgl/lvgl.git
        GIT_TAG ${MPM_LVGL_TAG}
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(lvgl)

    if(TARGET lvgl)
        target_link_libraries(lvgl PUBLIC ${MPM_SDL_TARGET})

        get_target_property(_mpm_lvgl_sources lvgl SOURCES)
        foreach(_mpm_lvgl_source IN LISTS _mpm_lvgl_sources)
            if(_mpm_lvgl_source MATCHES "\\.S$")
                list(REMOVE_ITEM _mpm_lvgl_sources "${_mpm_lvgl_source}")
            endif()
        endforeach()
        set_property(TARGET lvgl PROPERTY SOURCES "${_mpm_lvgl_sources}")
    endif()
endif()

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

target_include_directories(mini_device
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

if(MPM_DEVICE_UI)
    target_sources(mini_device PRIVATE
        ui/config_ui.c
        ui/config_bridge.cpp
    )
    target_compile_definitions(mini_device PRIVATE MPM_DEVICE_UI=1)
    target_include_directories(mini_device PRIVATE ${lvgl_SOURCE_DIR}/src)
    target_link_libraries(mini_device PRIVATE lvgl::lvgl ${MPM_SDL_TARGET})
    if(UNIX AND NOT APPLE)
        target_link_libraries(mini_device PRIVATE pthread dl m)
    endif()
else()
    target_compile_definitions(mini_device PRIVATE MPM_DEVICE_UI=0)
endif()

target_link_libraries(mini_device PRIVATE mpm_common)

set_target_properties(mini_device PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/device
)

option(MPM_BUILD_CONFIG_TEST "Build device config simulation test utility" ON)
if(MPM_BUILD_CONFIG_TEST)
    add_executable(mpm_config_test src/config_sim_test.cpp src/device_config.cpp src/ecg_sim.cpp src/spo2_sim.cpp)
    target_include_directories(mpm_config_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(mpm_config_test PRIVATE mpm_common)
    set_target_properties(mpm_config_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/device
    )
endif()