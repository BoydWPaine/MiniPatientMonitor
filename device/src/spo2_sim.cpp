#include "spo2_sim.h"

#include "sim_common.h"

#include <random>

namespace mpm::device {

Spo2Sim::Spo2Sim(DeviceConfigStore& config) : config_(config) {}

monitor::Spo2Packet Spo2Sim::next_packet()
{
    DeviceConfig cfg = config_.snapshot();
    monitor::Spo2Packet packet;

    if (!cfg.spo2_fixed) {
        thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<uint32_t> dist(97, 99);
        cfg.spo2 = dist(rng);
    }

    packet.set_spo2(cfg.spo2);
    packet.set_pr(cfg.pr);

    const double time_s =
        static_cast<double>(sample_index_ * kSamplePeriodMs) / 1000.0;
    const double frequency_hz = static_cast<double>(cfg.pr) / 60.0;
    packet.add_pleth_wave(sine_sample(frequency_hz, 0.0, time_s));

    ++sample_index_;
    return packet;
}

}  // namespace mpm::device