#ifndef MPM_DEVICE_CONFIG_BRIDGE_H
#define MPM_DEVICE_CONFIG_BRIDGE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MpmDeviceConfigStore MpmDeviceConfigStore;

MpmDeviceConfigStore* mpm_config_store_create(void);
void mpm_config_store_destroy(MpmDeviceConfigStore* store);

uint32_t mpm_config_get_hr(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_pr(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_spo2(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_resp_rate(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_temperature(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_nibp_sys(const MpmDeviceConfigStore* store);
uint32_t mpm_config_get_nibp_dia(const MpmDeviceConfigStore* store);

bool mpm_config_set_hr(MpmDeviceConfigStore* store, uint32_t hr);
bool mpm_config_set_pr(MpmDeviceConfigStore* store, uint32_t pr);
bool mpm_config_set_spo2(MpmDeviceConfigStore* store, uint32_t spo2);
bool mpm_config_set_resp_rate(MpmDeviceConfigStore* store, uint32_t resp_rate);
bool mpm_config_set_temperature(MpmDeviceConfigStore* store, uint32_t temperature);
bool mpm_config_set_nibp(MpmDeviceConfigStore* store, uint32_t sys, uint32_t dia);

uint32_t mpm_config_calc_map(uint32_t sys, uint32_t dia);
void mpm_config_disable_spo2_module(MpmDeviceConfigStore* store);

#ifdef __cplusplus
}

#include "device_config.h"

mpm::device::DeviceConfigStore& mpm_config_native(MpmDeviceConfigStore* store);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif