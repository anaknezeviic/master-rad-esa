#include "memory.hpp"

#ifdef _WIN32

#include <windows.h>
#include <psapi.h>

std::size_t get_peak_memory_bytes()
{
    PROCESS_MEMORY_COUNTERS_EX memory_info;

    if (
        GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(
                &memory_info
            ),
            sizeof(memory_info)
        )
    ) {
        return static_cast<std::size_t>(
            memory_info.PeakWorkingSetSize
        );
    }

    return 0;
}

#else

std::size_t get_peak_memory_bytes()
{
    return 0;
}

#endif