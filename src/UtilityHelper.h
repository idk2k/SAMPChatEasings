#ifndef UTILITY_HELPER_H
#define UTILITY_HELPER_H

#include <cstdint>
#include <iostream>

#include "MinHook/MinHook.h"

namespace utils {
    void MH_CreateAndEnableHook(std::uintptr_t&& TargetAddress, LPVOID pDetour, LPVOID* ppOriginal);
    void protect_safe_memory_copy(void* destination, void* source, std::size_t size);
    void protect_safe_memory_set(void* destination, int value, std::size_t size);
}

#endif  // UTILITY_HELPER_H