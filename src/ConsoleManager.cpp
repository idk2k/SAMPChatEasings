#include <wincon.h>
#include <cstdio>
#include "ConsoleManager.h"

ConsoleManager& ConsoleManager::get_instance() {
    static ConsoleManager console_mgr_inst{};
    return console_mgr_inst;
}

void ConsoleManager::create_console() {
    if (!AllocConsole())
        return;
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
}
