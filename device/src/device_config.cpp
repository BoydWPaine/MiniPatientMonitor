#include "device_config.h"

namespace mpm::device {

bool validate_hr(uint32_t hr) { return hr >= 38 && hr <= 188; }
bool validate_pr(uint32_t pr) { return pr >= 38 && pr <= 188; }
bool validate_spo2(uint32_t spo2) { return spo2 >= 50 && spo2 <= 99; }
bool validate_resp_rate(uint32_t resp_rate) { return resp_rate >= 6 && resp_rate <= 38; }
bool validate_temperature(uint32_t temperature) { return temperature >= 288 && temperature <= 420; }
bool validate_nibp_sys(uint32_t sys) { return sys > 84 && sys <= 180; }
bool validate_nibp_dia(uint32_t dia) { return dia >= 50 && dia <= 84; }

DeviceConfig DeviceConfigStore::snapshot() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
}

void DeviceConfigStore::update(const DeviceConfig& config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

bool DeviceConfigStore::set_hr(uint32_t hr)
{
    if (!validate_hr(hr)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.hr = hr;
    config_.hr_fixed = true;
    return true;
}

bool DeviceConfigStore::set_pr(uint32_t pr)
{
    if (!validate_pr(pr)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.pr = pr;
    config_.pr_fixed = true;
    return true;
}

bool DeviceConfigStore::set_spo2(uint32_t spo2)
{
    if (!validate_spo2(spo2)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.spo2 = spo2;
    config_.spo2_fixed = true;
    return true;
}

bool DeviceConfigStore::set_resp_rate(uint32_t resp_rate)
{
    if (!validate_resp_rate(resp_rate)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.resp_rate = resp_rate;
    config_.resp_fixed = true;
    return true;
}

bool DeviceConfigStore::set_temperature(uint32_t temperature)
{
    if (!validate_temperature(temperature)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.temperature = temperature;
    config_.temp_fixed = true;
    return true;
}

bool DeviceConfigStore::set_nibp(uint32_t sys, uint32_t dia)
{
    if (!validate_nibp_sys(sys) || !validate_nibp_dia(dia) || dia >= sys) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_.nibp_sys = sys;
    config_.nibp_dia = dia;
    config_.nibp_fixed = true;
    return true;
}

}  // namespace mpm::device