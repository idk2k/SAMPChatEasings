#include "ConsoleManager.h"

#include <cstdio>

ConsoleManager& ConsoleManager::get_instance() {
    static ConsoleManager console_mgr_inst{};
    return console_mgr_inst;
}

[[nodiscard]] BOOL ConsoleManager::create_console() {
    BOOL status = AllocConsole();
    if (!status) return status;
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    return status;
}
