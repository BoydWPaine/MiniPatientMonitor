#include "resp_sim.h"

#include "sim_common.h"

namespace mpm::device {

RespSim::RespSim(DeviceConfigStore& config) : config_(config) {}

monitor::RespPacket RespSim::next_packet()
{
    const DeviceConfig cfg = config_.snapshot();
    monitor::RespPacket packet;
    packet.set_resp_rate(cfg.resp_rate);

    const double time_s =
        static_cast<double>(sample_index_ * kSamplePeriodMs) / 1000.0;
    const double frequency_hz = static_cast<double>(cfg.resp_rate) / 60.0;
    packet.add_resp_wave(sine_sample(frequency_hz, 0.5, time_s));

    ++sample_index_;
    return packet;
}

}  // namespace mpm::device