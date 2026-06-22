#include "nibp_sim.h"

#include <random>

namespace mpm::device {

NibpSim::NibpSim(DeviceConfigStore& config) : config_(config) {}

monitor::NibpPacket NibpSim::measure()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::NibpPacket packet;

    uint32_t sys = cfg.nibp_sys;
    uint32_t dia = cfg.nibp_dia;

    if (!cfg.nibp_fixed) {
        thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<uint32_t> sys_dist(120, 128);
        std::uniform_int_distribution<uint32_t> dia_dist(76, 84);
        sys = sys_dist(rng);
        dia = dia_dist(rng);
    }

    packet.set_nibp_sys(sys);
    packet.set_nibp_dia(dia);
    packet.set_nibp_mean((sys + (2U * dia)) / 3U);
    return packet;
}

}  // namespace mpm::device