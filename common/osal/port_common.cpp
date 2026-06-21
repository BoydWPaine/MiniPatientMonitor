#include "osal/osal.h"

#include <chrono>
#include <thread>

uint32_t osal_get_tick_ms(void)
{
    using clock = std::chrono::steady_clock;
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        clock::now().time_since_epoch());
    return static_cast<uint32_t>(elapsed.count());
}

void osal_thread_sleep_ms(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}