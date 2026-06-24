#ifndef MPM_DEVICE_SIM_COMMON_H
#define MPM_DEVICE_SIM_COMMON_H

#include <cmath>
#include <cstdint>

namespace mpm::device {

constexpr int32_t kWaveAmplitude = 2048;
constexpr uint32_t kSamplePeriodMs = 40;
constexpr uint32_t kSamplesPerPacket = 4;
constexpr double kPi = 3.14159265358979323846;

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

inline int32_t sine_sample(double frequency_hz, double phase_rad, double time_s)
{
    const double value = std::sin((2.0 * kPi * frequency_hz * time_s) + phase_rad);
    return clamp_wave(static_cast<int32_t>(value * static_cast<double>(kWaveAmplitude)));
}

}  // namespace mpm::device

#endif