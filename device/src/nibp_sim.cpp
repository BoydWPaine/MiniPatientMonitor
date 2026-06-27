#include "nibp_sim.h"

namespace mpm::device {

NibpSim::NibpSim(DeviceConfigStore& config) : config_(config) {}

monitor::NibpPacket NibpSim::measure()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::NibpPacket packet;

    const uint32_t sys = cfg.nibp_sys;
    const uint32_t dia = cfg.nibp_dia;

    packet.set_nibp_sys(sys);
    packet.set_nibp_dia(dia);
    packet.set_nibp_mean((sys + (2U * dia)) / 3U);
    return packet;
}

}  // namespace mpm::device