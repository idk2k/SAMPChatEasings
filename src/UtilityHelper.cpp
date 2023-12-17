#include "UtilityHelper.h"

auto utils::MH_CreateAndEnableHook(std::uintptr_t&& TargetAddress, LPVOID pDetour, LPVOID* ppOriginal) -> void {
    MH_STATUS create_hook_status = MH_CreateHook(reinterpret_cast<LPVOID>(TargetAddress), pDetour, ppOriginal);
    if (create_hook_status != MH_OK) {
        std::cout << "[ERROR]: MH_CreateHook() failed, return code: " << create_hook_status << std::endl;
        return;
    }
    MH_STATUS enable_hook_status = MH_EnableHook(reinterpret_cast<LPVOID>(TargetAddress));
    if (enable_hook_status != MH_OK) {
        std::cout << "[ERROR]: MH_EnableHook() failed, return code: " << enable_hook_status << std::endl;
        return;
    }
}

auto utils::protect_safe_memory_copy(void* destination, void* source, std::size_t size) -> void {
    DWORD oldProtect{};
    VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(destination, source, size);
    VirtualProtect(source, size, oldProtect, &oldProtect);
}

auto utils::protect_safe_memory_set(void* destination, int value, std::size_t size) -> void {
    DWORD oldProtect{};
    VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(destination, value, size);
    VirtualProtect(destination, size, oldProtect, &oldProtect);
}