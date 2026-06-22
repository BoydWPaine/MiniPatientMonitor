#ifndef MPM_DEVICE_SPO2_SIM_H
#define MPM_DEVICE_SPO2_SIM_H

#include "device_config.h"
#include "monitor.pb.h"

namespace mpm::device {

class Spo2Sim {
public:
    explicit Spo2Sim(DeviceConfigStore& config);

    monitor::Spo2Packet next_packet();

private:
    DeviceConfigStore& config_;
    uint64_t sample_index_ = 0;
};

}  // namespace mpm::device

#endif