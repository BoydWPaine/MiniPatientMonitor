#ifndef MPM_DEVICE_RESP_SIM_H
#define MPM_DEVICE_RESP_SIM_H

#include "device_config.h"
#include "monitor.pb.h"

namespace mpm::device {

class RespSim {
public:
    explicit RespSim(DeviceConfigStore& config);

    monitor::RespPacket next_packet();

private:
    DeviceConfigStore& config_;
    uint64_t sample_index_ = 0;
};

}  // namespace mpm::device

#endif