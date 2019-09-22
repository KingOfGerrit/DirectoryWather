#include "pch.h"

#include <windows.h>

#include <iostream>
#include <string>

#include "timer.h"
#include "tree_snapshot.h"

int main(int argc, char *argv[])
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	if (argc < 2)
	{
		std::cout << "Please, enter the directory name to monitor." << std::endl;
		return 1;
	}

	std::string path{ argv[1] };

	if (!std::filesystem::is_directory(path))
	{
		std::cout << "Error! Invalid directory." << std::endl;
		return 2;
	}

	TreeSnapshot fw{ path };
	auto action = [](std::string path_to_watch, Status status) -> void {
		// Process only regular files and directories, all other file types are ignored
		if (!std::filesystem::is_directory(std::filesystem::path(path_to_watch)) &&
			!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) &&
			status != Status::erased)
		{
			return;
		}

		switch (status)
		{
		case Status::created:
			std::cout << "Created: " << path_to_watch << '\n';
			break;
		case Status::modified:
			std::cout << "Modified: " << path_to_watch << '\n';
			break;
		case Status::erased:
			std::cout << "Erased: " << path_to_watch << '\n';
			break;
		default:
			std::cout << "Error! Unknown status.\n";
		}
	};

	Timer t;

	std::cout << "Start monitor dir \"" << path << '"' << std::endl;

	t.setInterval(100, std::bind(&TreeSnapshot::checkUpdates, &fw, action) );
	t.setTimeout(60000, true, [&t]() { t.stop(); });

	std::cout << "Stop" << std::endl;
}
