#include "net_server.h"

#include "device_config.h"
#include "osal/osal.h"

#include <cstdio>
#include <cstring>

#if MPM_DEVICE_UI
#include "config_bridge.h"
#include "config_ui.h"
#endif

namespace {

#if MPM_DEVICE_UI
struct UiThreadContext {
    MpmDeviceConfigStore* store = nullptr;
};

void ui_thread_main(void* arg)
{
    auto* ctx = static_cast<UiThreadContext*>(arg);
    if (config_ui_run(ctx->store) != 0) {
        std::fprintf(stderr, "[device] LVGL UI exited with error\n");
    }
    delete ctx;
}
#endif

}  // namespace

int main(int argc, char* argv[])
{
    bool disable_spo2 = false;
    bool enable_ui = true;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--disable-spo2") == 0) {
            disable_spo2 = true;
        } else if (std::strcmp(argv[i], "--no-ui") == 0) {
            enable_ui = false;
        }
    }

    std::printf("MiniPatientMonitor device (TCP server)\n");

#if MPM_DEVICE_UI
    MpmDeviceConfigStore* bridge_store = mpm_config_store_create();
    mpm::device::DeviceConfigStore& config_store = mpm_config_native(bridge_store);

    if (disable_spo2) {
        mpm_config_disable_spo2_module(bridge_store);
        std::printf("[device] SpO2 module disabled (TC-IT-09 test mode)\n");
    }

    if (enable_ui) {
        auto* ui_ctx = new UiThreadContext{bridge_store};
        const osal_thread_t ui_thread =
            osal_thread_create(ui_thread_main, ui_ctx, 1, 1024U * 256U);
        if (ui_thread == OSAL_THREAD_INVALID) {
            std::fprintf(stderr, "[device] failed to start LVGL UI thread\n");
            delete ui_ctx;
        } else {
            std::printf("[device] LVGL config UI started (320x240)\n");
            osal_thread_sleep_ms(200);
        }
    }

    const int rc = mpm::device::run_server(MPM_TCP_HOST,
                                           static_cast<uint16_t>(MPM_TCP_PORT),
                                           config_store,
                                           false);
    mpm_config_store_destroy(bridge_store);
    return rc;
#else
    mpm::device::DeviceConfigStore config_store;
    if (disable_spo2) {
        mpm::device::DeviceConfig cfg = config_store.snapshot();
        cfg.spo2_enabled = false;
        config_store.update(cfg);
        std::printf("[device] SpO2 module disabled (TC-IT-09 test mode)\n");
    }

    return mpm::device::run_server(MPM_TCP_HOST,
                                   static_cast<uint16_t>(MPM_TCP_PORT),
                                   config_store,
                                   disable_spo2);
#endif
}