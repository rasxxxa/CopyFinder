
import CopyFinderModule;

#include <iostream>
#include <filesystem>
using namespace std::filesystem;

const std::string GetDesktopDirectorium()
{
	static std::string desktopDirectory;

	if (!desktopDirectory.empty())
		return desktopDirectory;

	for (auto entry : directory_iterator("C:\\Users\\"))
	{
		std::string tryDirectory = entry.path().string();
		tryDirectory.append("\\Desktop");
		//if (std::filesystem::is)
		//{
		//	desktopDirectory = tryDirectory;
		//	return desktopDirectory;
		//}
		
	}

}

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstdio>


import CopyFinderModule;
auto main(int nargs, char**args) -> int
{
	std::string currentPath = args[0];
	auto pos = currentPath.find_last_of('\\');
	currentPath = currentPath.substr(0, pos);
	while (true)
	{
		system("cls");
		std::cout << "Current path is: " << std::endl << currentPath << std::endl;
		std::cout << "Press 1. to change current path" << std::endl;
		std::cout << "Press 2. List all duplicates in folder" << std::endl;
		std::cout << "Press 3. List all duplicates in folder recursively" << std::endl;

		int in;
		std::cin >> in;

		switch (in)
		{
		case 1: {
		
			bool ok = true;
			do
			{
				std::cout << "Type in new FULL path" << std::endl;
				std::string newPath;
				std::cin >> newPath;
				
				if (!std::filesystem::exists(newPath))
				{
					std::cout << "Please type in ok path!" << std::endl;
					ok = false;
				}
				else
				{
					currentPath = newPath;
					ok = true;
				}
			} while (!ok);
		}; break;
		case 2: 
		{ 
			ListAllDuplicates(false, currentPath); 
			std::cout << "All duplicates listed in " << currentPath << "/copies.txt" << std::endl;
			std::cout << "Press 1. to delete those duplicates" << std::endl;
			int inDelete;
			std::cin >> inDelete;
			switch (inDelete)
			{
			case 1: {
			
				DeleteAllCopies();
			
			}; break;
			default: break;
			}
		}
		default: std::cout << "WRONG INPUT!" << std::endl;
		}
	}
}