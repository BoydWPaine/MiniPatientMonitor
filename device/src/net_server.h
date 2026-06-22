#ifndef MPM_DEVICE_NET_SERVER_H
#define MPM_DEVICE_NET_SERVER_H

#include <cstdint>

namespace mpm::device {

int run_server(const char* host, uint16_t port, bool disable_spo2 = false);

}  // namespace mpm::device

#endif