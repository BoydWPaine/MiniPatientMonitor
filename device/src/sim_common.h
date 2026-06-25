#ifndef MPM_DEVICE_SIM_COMMON_H
#define MPM_DEVICE_SIM_COMMON_H

#include <cstdint>

namespace mpm::device {

constexpr int32_t kWaveAmplitude = 2048;
constexpr uint32_t kSamplePeriodMs = 40;
constexpr uint32_t kSamplesPerPacket = 4;

inline int32_t clamp_wave(int32_t value)
{
    if (value > kWaveAmplitude) {
        return kWaveAmplitude;
    }
    if (value < -kWaveAmplitude) {
        return -kWaveAmplitude;
    }
    return value;
}

}  // namespace mpm::device

#endif