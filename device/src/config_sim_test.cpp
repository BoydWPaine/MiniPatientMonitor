#include "device_config.h"
#include "ecg_sim.h"
#include "sim_common.h"
#include "spo2_sim.h"

#include <cstdio>

int main()
{
    mpm::device::DeviceConfigStore store;
    if (!store.set_hr(148)) {
        std::fprintf(stderr, "set_hr failed\n");
        return 1;
    }

    mpm::device::EcgSim ecg(store);
    const monitor::EcgPacket packet = ecg.next_packet();
    if (packet.hr() != 148) {
        std::fprintf(stderr, "expected hr=148, got %u\n", packet.hr());
        return 1;
    }
    if (packet.ecg_lead_ii_size() != static_cast<int>(mpm::device::kSamplesPerPacket)) {
        std::fprintf(stderr,
                     "expected %u ECG samples, got %d\n",
                     mpm::device::kSamplesPerPacket,
                     packet.ecg_lead_ii_size());
        return 1;
    }
    for (int i = 0; i < packet.ecg_lead_ii_size(); ++i) {
        const int32_t sample = packet.ecg_lead_ii(i);
        if (sample > mpm::device::kWaveAmplitude || sample < -mpm::device::kWaveAmplitude) {
            std::fprintf(stderr, "ECG sample out of range: %d\n", sample);
            return 1;
        }
    }

    if (!store.set_spo2(88)) {
        std::fprintf(stderr, "set_spo2 failed\n");
        return 1;
    }

    mpm::device::Spo2Sim spo2(store);
    const monitor::Spo2Packet spo2_packet = spo2.next_packet();
    if (spo2_packet.spo2() != 88) {
        std::fprintf(stderr, "expected fixed spo2=88, got %u\n", spo2_packet.spo2());
        return 1;
    }

    const monitor::Spo2Packet spo2_packet2 = spo2.next_packet();
    if (spo2_packet2.spo2() != 88) {
        std::fprintf(stderr, "expected stable spo2=88, got %u\n", spo2_packet2.spo2());
        return 1;
    }

    std::printf("config_sim_test OK (hr=%u spo2=%u)\n", packet.hr(), spo2_packet.spo2());
    return 0;
}