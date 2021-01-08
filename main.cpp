#include "Interpreter.h"

#include <iostream>
#include <cstring>
#include <vector>

Interpreter interpreter;

enum class WorkMode
{
	Interactive = 0,
	File,
	Gui
};

struct
{
	WorkMode wm = WorkMode::Interactive;
	std::string filename;
} globalArgs;

static void argParse(int ac, char **av)
{
	if (ac == 1) return;

	for (int i = 1; i < ac; i++)
	{
		if (strcmp(av[i], "-h") == 0)
		{
			std::cerr << "Usage: " << av[0] << " [-h] (help) | [-g] (GUI) | [-f file] (filemode) " << std::endl;
			exit(0);
		}

		if (strcmp(av[i], "-g") == 0)
		{
			globalArgs.wm = WorkMode::Gui;
			return;
		}

		if (strcmp(av[i], "-f") == 0)
		{
			if (!av[i + 1])
			{
				std::cerr << "Usage: " << av[0] << " [-h] (help) | [-g] (GUI) | [-f file] (filemode) " << std::endl;
				exit(0);
			}
			globalArgs.wm = WorkMode::File;
			globalArgs.filename = av[i + 1];
			return;
		}

	}
}

void startGui();

int main(int ac, char **av) {
	argParse(ac, av);

	if (globalArgs.wm == WorkMode::Interactive)
	{
		try {
			interpreter.startInteractive();
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return 0;
		}
	}
	else if (globalArgs.wm == WorkMode::File)
	{
		try {
			interpreter.startFile(globalArgs.filename);
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return 0;
		}
	}
	else if (globalArgs.wm == WorkMode::Gui)
	{
		startGui();
	}
	else
		std::cerr << "Unknown error" << std::endl;
}

