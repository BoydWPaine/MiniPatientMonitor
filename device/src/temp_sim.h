#ifndef MPM_DEVICE_TEMP_SIM_H
#define MPM_DEVICE_TEMP_SIM_H

#include "device_config.h"
#include "monitor.pb.h"

namespace mpm::device {

class TempSim {
public:
    explicit TempSim(DeviceConfigStore& config);

    monitor::TempPacket next_packet();

private:
    DeviceConfigStore& config_;
    uint64_t sample_index_ = 0;
};

}  // namespace mpm::device

#endif