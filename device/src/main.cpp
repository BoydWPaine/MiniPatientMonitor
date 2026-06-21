#include "net_server.h"

#include <cstdio>

int main()
{
    std::printf("MiniPatientMonitor device (TCP server)\n");
    return mpm::device::run_server(MPM_TCP_HOST, static_cast<uint16_t>(MPM_TCP_PORT));
}