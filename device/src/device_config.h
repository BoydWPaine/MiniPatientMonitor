#ifndef MPM_DEVICE_CONFIG_H
#define MPM_DEVICE_CONFIG_H

#include <cstdint>
#include <mutex>

namespace mpm::device {

struct DeviceConfig {
    uint32_t hr = 60;
    uint32_t pr = 60;
    uint32_t spo2 = 98;
    uint32_t resp_rate = 20;
    uint32_t temperature = 365;
    uint32_t nibp_sys = 120;
    uint32_t nibp_dia = 80;

    bool ecg_enabled = true;
    bool spo2_enabled = true;
    bool resp_enabled = true;
    bool temp_enabled = true;
    bool nibp_enabled = true;
};

class DeviceConfigStore {
public:
    DeviceConfig snapshot() const;
    void update(const DeviceConfig& config);

    bool set_hr(uint32_t hr);
    bool set_pr(uint32_t pr);
    bool set_spo2(uint32_t spo2);
    bool set_resp_rate(uint32_t resp_rate);
    bool set_temperature(uint32_t temperature);
    bool set_nibp(uint32_t sys, uint32_t dia);

private:
    mutable std::mutex mutex_;
    DeviceConfig config_;
};

bool validate_hr(uint32_t hr);
bool validate_pr(uint32_t pr);
bool validate_spo2(uint32_t spo2);
bool validate_resp_rate(uint32_t resp_rate);
bool validate_temperature(uint32_t temperature);
bool validate_nibp_sys(uint32_t sys);
bool validate_nibp_dia(uint32_t dia);

}  // namespace mpm::device

#endif