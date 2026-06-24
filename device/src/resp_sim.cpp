#include "resp_sim.h"

#include "sim_common.h"
#include "wave_demo_data.h"

namespace mpm::device {

RespSim::RespSim(DeviceConfigStore& config) : config_(config) {}

monitor::RespPacket RespSim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::RespPacket packet;
    packet.set_resp_rate(cfg.resp_rate);

    for (uint32_t i = 0; i < kSamplesPerPacket; ++i) {
        const uint32_t idx =
            (static_cast<uint32_t>(sample_index_) + i) % kDemoSampleCount;
        packet.add_resp_wave(clamp_wave(kWaveResp[idx]));
    }

    sample_index_ = (sample_index_ + kSamplesPerPacket) % kDemoSampleCount;
    return packet;
}

}  // namespace mpm::device