#include "temp_sim.h"

#include "sim_common.h"
#include "wave_demo_data.h"

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

    for (uint32_t i = 0; i < kSamplesPerPacket; ++i) {
        const uint32_t idx =
            (static_cast<uint32_t>(sample_index_) + i) % kDemoSampleCount;
        packet.add_temp_wave(clamp_wave(kWaveTemp[idx]));
    }

    sample_index_ = (sample_index_ + kSamplesPerPacket) % kDemoSampleCount;
    return packet;
}

}  // namespace mpm::device