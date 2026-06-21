#ifndef MPM_HOST_NET_CLIENT_H
#define MPM_HOST_NET_CLIENT_H

#include <cstdint>

namespace mpm::host {

int run_client(const char* host, uint16_t port, uint32_t max_frames);

}  // namespace mpm::host

#endif