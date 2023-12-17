#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <windows.h>

class ConsoleManager {
public:
    static ConsoleManager& get_instance();
    [[nodiscard]] BOOL create_console();
};

#endif  // CONSOLE_MANAGER_H