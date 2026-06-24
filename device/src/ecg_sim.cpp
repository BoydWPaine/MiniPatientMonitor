#include "ecg_sim.h"

#include "sim_common.h"
#include "wave_demo_data.h"

namespace mpm::device {

EcgSim::EcgSim(DeviceConfigStore& config) : config_(config) {}

monitor::EcgPacket EcgSim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::EcgPacket packet;
    packet.set_hr(cfg.hr);

    for (uint32_t i = 0; i < kSamplesPerPacket; ++i) {
        const uint32_t idx =
            (static_cast<uint32_t>(sample_index_) + i) % kDemoSampleCount;
        packet.add_ecg_lead_i(clamp_wave(kWaveLeadI[idx]));
        packet.add_ecg_lead_ii(clamp_wave(kWaveLeadII[idx]));
        packet.add_ecg_lead_iii(clamp_wave(kWaveLeadIII[idx]));
        packet.add_ecg_lead_avr(clamp_wave(kWaveLeadAvr[idx]));
        packet.add_ecg_lead_avl(clamp_wave(kWaveLeadAvl[idx]));
        packet.add_ecg_lead_avf(clamp_wave(kWaveLeadAvf[idx]));
        packet.add_ecg_lead_v1(clamp_wave(kWaveLeadV1[idx]));
        packet.add_ecg_lead_v2(clamp_wave(kWaveLeadV2[idx]));
        packet.add_ecg_lead_v3(clamp_wave(kWaveLeadV3[idx]));
        packet.add_ecg_lead_v4(clamp_wave(kWaveLeadV4[idx]));
        packet.add_ecg_lead_v5(clamp_wave(kWaveLeadV5[idx]));
        packet.add_ecg_lead_v6(clamp_wave(kWaveLeadV6[idx]));
    }

    sample_index_ = (sample_index_ + kSamplesPerPacket) % kDemoSampleCount;
    return packet;
}

}  // namespace mpm::device