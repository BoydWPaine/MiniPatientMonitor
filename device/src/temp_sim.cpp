#include "temp_sim.h"

#include "sim_common.h"
#include "wave_demo_data.h"

namespace mpm::device {

TempSim::TempSim(DeviceConfigStore& config) : config_(config) {}

monitor::TempPacket TempSim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::TempPacket packet;

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