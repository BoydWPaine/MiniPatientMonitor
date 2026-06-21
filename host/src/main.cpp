#include "net_client.h"

#include <cstdio>

int main()
{
    std::printf("MiniPatientMonitor host (TCP client)\n");
    return mpm::host::run_client(MPM_TCP_HOST, static_cast<uint16_t>(MPM_TCP_PORT), 3);
}