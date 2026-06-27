#include "spo2_sim.h"

#include "sim_common.h"
#include "wave_demo_data.h"

namespace mpm::device {

Spo2Sim::Spo2Sim(DeviceConfigStore& config) : config_(config) {}

monitor::Spo2Packet Spo2Sim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::Spo2Packet packet;

    packet.set_spo2(cfg.spo2);
    packet.set_pr(cfg.pr);

    for (uint32_t i = 0; i < kSamplesPerPacket; ++i) {
        const uint32_t idx =
            (static_cast<uint32_t>(sample_index_) + i) % kDemoSampleCount;
        packet.add_pleth_wave(clamp_wave(kWavePleth[idx]));
    }

    sample_index_ = (sample_index_ + kSamplesPerPacket) % kDemoSampleCount;
    return packet;
}

}  // namespace mpm::device