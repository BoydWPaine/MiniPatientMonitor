#include "config_bridge.h"

#include "device_config.h"

struct MpmDeviceConfigStore {
    mpm::device::DeviceConfigStore store;
};

MpmDeviceConfigStore* mpm_config_store_create()
{
    return new MpmDeviceConfigStore();
}

void mpm_config_store_destroy(MpmDeviceConfigStore* store)
{
    delete store;
}

mpm::device::DeviceConfigStore& native_store(const MpmDeviceConfigStore* store)
{
    return const_cast<MpmDeviceConfigStore*>(store)->store;
}

uint32_t mpm_config_get_hr(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().hr;
}

uint32_t mpm_config_get_pr(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().pr;
}

uint32_t mpm_config_get_spo2(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().spo2;
}

uint32_t mpm_config_get_resp_rate(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().resp_rate;
}

uint32_t mpm_config_get_temperature(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().temperature;
}

uint32_t mpm_config_get_nibp_sys(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().nibp_sys;
}

uint32_t mpm_config_get_nibp_dia(const MpmDeviceConfigStore* store)
{
    return native_store(store).snapshot().nibp_dia;
}

bool mpm_config_set_hr(MpmDeviceConfigStore* store, uint32_t hr)
{
    return native_store(store).set_hr(hr);
}

bool mpm_config_set_pr(MpmDeviceConfigStore* store, uint32_t pr)
{
    return native_store(store).set_pr(pr);
}

bool mpm_config_set_spo2(MpmDeviceConfigStore* store, uint32_t spo2)
{
    return native_store(store).set_spo2(spo2);
}

bool mpm_config_set_resp_rate(MpmDeviceConfigStore* store, uint32_t resp_rate)
{
    return native_store(store).set_resp_rate(resp_rate);
}

bool mpm_config_set_temperature(MpmDeviceConfigStore* store, uint32_t temperature)
{
    return native_store(store).set_temperature(temperature);
}

bool mpm_config_set_nibp(MpmDeviceConfigStore* store, uint32_t sys, uint32_t dia)
{
    return native_store(store).set_nibp(sys, dia);
}

uint32_t mpm_config_calc_map(uint32_t sys, uint32_t dia)
{
    return (sys + (2U * dia)) / 3U;
}

void mpm_config_disable_spo2_module(MpmDeviceConfigStore* store)
{
    auto cfg = native_store(store).snapshot();
    cfg.spo2_enabled = false;
    native_store(store).update(cfg);
}

mpm::device::DeviceConfigStore& mpm_config_native(MpmDeviceConfigStore* store)
{
    return native_store(store);
}