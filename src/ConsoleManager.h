#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

class ConsoleManager {
public:
	static ConsoleManager& get_instance();
	void create_console();
};

#endif // CONSOLE_MANAGER_H