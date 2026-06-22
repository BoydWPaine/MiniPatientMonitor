#ifndef MPM_DEVICE_NIBP_SIM_H
#define MPM_DEVICE_NIBP_SIM_H

#include "device_config.h"
#include "monitor.pb.h"

namespace mpm::device {

class NibpSim {
public:
    explicit NibpSim(DeviceConfigStore& config);

    monitor::NibpPacket measure();

private:
    DeviceConfigStore& config_;
};

}  // namespace mpm::device

#endif