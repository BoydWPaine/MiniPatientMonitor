#include "temp_sim.h"

#include "sim_common.h"

#include <random>

namespace mpm::device {

TempSim::TempSim(DeviceConfigStore& config) : config_(config) {}

monitor::TempPacket TempSim::next_packet()
{
    DeviceConfig cfg = config_.snapshot();
    monitor::TempPacket packet;

    if (!cfg.temp_fixed) {
        thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<uint32_t> dist(362, 368);
        cfg.temperature = dist(rng);
    }

    packet.set_temperature(cfg.temperature);

    const double time_s =
        static_cast<double>(sample_index_ * kSamplePeriodMs) / 1000.0;
    const double slow_wave =
        std::sin(2.0 * kPi * 0.05 * time_s) * static_cast<double>(kWaveAmplitude) * 0.15;
    packet.add_temp_wave(clamp_wave(static_cast<int32_t>(slow_wave)));

    ++sample_index_;
    return packet;
}

}  // namespace mpm::device