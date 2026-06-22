#ifndef MPM_DEVICE_ECG_SIM_H
#define MPM_DEVICE_ECG_SIM_H

#include "device_config.h"
#include "monitor.pb.h"

namespace mpm::device {

class EcgSim {
public:
    explicit EcgSim(DeviceConfigStore& config);

    monitor::EcgPacket next_packet();

private:
    DeviceConfigStore& config_;
    uint64_t sample_index_ = 0;
};

}  // namespace mpm::device

#endif