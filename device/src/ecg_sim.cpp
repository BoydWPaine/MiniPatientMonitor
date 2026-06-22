#include "ecg_sim.h"

#include "sim_common.h"

namespace mpm::device {
namespace {

void append_sample(monitor::EcgPacket& packet, int field_number, int32_t sample)
{
    switch (field_number) {
    case 2:
        packet.add_ecg_lead_i(sample);
        break;
    case 3:
        packet.add_ecg_lead_ii(sample);
        break;
    case 4:
        packet.add_ecg_lead_iii(sample);
        break;
    case 5:
        packet.add_ecg_lead_avr(sample);
        break;
    case 6:
        packet.add_ecg_lead_avl(sample);
        break;
    case 7:
        packet.add_ecg_lead_avf(sample);
        break;
    case 8:
        packet.add_ecg_lead_v1(sample);
        break;
    case 9:
        packet.add_ecg_lead_v2(sample);
        break;
    case 10:
        packet.add_ecg_lead_v3(sample);
        break;
    case 11:
        packet.add_ecg_lead_v4(sample);
        break;
    case 12:
        packet.add_ecg_lead_v5(sample);
        break;
    case 13:
        packet.add_ecg_lead_v6(sample);
        break;
    default:
        break;
    }
}

}  // namespace

EcgSim::EcgSim(DeviceConfigStore& config) : config_(config) {}

monitor::EcgPacket EcgSim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::EcgPacket packet;
    packet.set_hr(cfg.hr);

    const double time_s =
        static_cast<double>(sample_index_ * kSamplePeriodMs) / 1000.0;
    const double frequency_hz = static_cast<double>(cfg.hr) / 60.0;

    struct LeadDef {
        int field_number;
        double phase_rad;
        double gain;
    };

    const LeadDef leads[] = {
        {2, 0.0, 0.85},
        {3, 0.4, 1.0},
        {4, 0.8, 0.75},
        {5, 2.6, -0.55},
        {6, 1.2, 0.65},
        {7, 1.8, 0.7},
        {8, 0.2, 0.9},
        {9, 0.6, 0.95},
        {10, 1.0, 0.8},
        {11, 1.4, 0.7},
        {12, 1.8, 0.65},
        {13, 2.2, 0.6},
    };

    for (const LeadDef& lead : leads) {
        const double value =
            std::sin((2.0 * kPi * frequency_hz * time_s) + lead.phase_rad) * lead.gain;
        append_sample(packet, lead.field_number,
                      clamp_wave(static_cast<int32_t>(value * kWaveAmplitude)));
    }

    ++sample_index_;
    return packet;
}

}  // namespace mpm::device