#include "device_config.h"
#include "ecg_sim.h"
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
        std::fprintf(stderr, "spo2 randomization did not stop: %u\n", spo2_packet2.spo2());
        return 1;
    }

    std::printf("config_sim_test OK (hr=%u spo2=%u)\n", packet.hr(), spo2_packet.spo2());
    return 0;
}