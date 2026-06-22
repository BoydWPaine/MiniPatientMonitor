#include "net_server.h"

#include <cstdio>
#include <cstring>

int main(int argc, char* argv[])
{
    bool disable_spo2 = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--disable-spo2") == 0) {
            disable_spo2 = true;
        }
    }

    std::printf("MiniPatientMonitor device (TCP server)\n");
    return mpm::device::run_server(MPM_TCP_HOST,
                                   static_cast<uint16_t>(MPM_TCP_PORT),
                                   disable_spo2);
}