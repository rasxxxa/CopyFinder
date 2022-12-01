import CopyFinderModule;

void StartProgram(char ** args)
{
	std::string currentPath = args[0];
	auto pos = currentPath.find_last_of('\\');
	currentPath = currentPath.substr(0, pos);
	std::unordered_map<std::string, bool> extensions;
	while (true)
	{
		system("cls");
		std::cout << "Current path is: " << std::endl << currentPath << std::endl;
		std::cout << "Press 1. to change current path" << std::endl;
		std::cout << "Press 2. to list duplicates" << std::endl;
		std::cout << "Press 3. To rename all files in folder" << std::endl;


		int in;
		std::cin >> in;

		switch (in)
		{
		case 1: 
		{

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
			bool ok = true;
			do
			{
				std::cout << "Press 1. to add extension to be checked!" << std::endl;
				std::cout << "Press 2. to list duplicates in current folder" << std::endl;
				std::cout << "Press 3. to list duplicates recursively" << std::endl;
				std::cout << "Press 4. to clear extensions" << std::endl;
				std::cout << "Press any other for back" << std::endl;
				if (!extensions.empty())
				{
					std::cout << "Extensions to be checked: " << std::endl;
					for (const auto& extension : extensions)
						std::cout << "\t" << extension.first << std::endl;
				}
				int value;
				std::cin >> value;

				switch (value)
				{
				case 1: 
				{
					std::cout << "Insert new extension" << std::endl;
					std::string in;
					std::cin >> in;
					extensions[in] = true;
				} break;
				case 2: 
				{
					ListAllDuplicates(extensions, false, currentPath);
					std::cout << "All duplicates listed in " << currentPath << " in files aranged by extension " << std::endl;
					std::cout << "Press 1. to delete those duplicates" << std::endl;
					std::cout << "Press any for back." << std::endl;
					int inDelete;
					std::cin >> inDelete;
					switch (inDelete)
					{
					case 1: {DeleteAllCopies();  };
					default:
						ok = false;
						break;
					}
				} break;
				case 3: 
				{
					ListAllDuplicates(extensions, true, currentPath);
					std::cout << "All duplicates listed in " << currentPath << " in files aranged by extension " << std::endl;
					std::cout << "Press 1. to delete those duplicates" << std::endl;
					std::cout << "Press any for back." << std::endl;
					int inDelete;
					std::cin >> inDelete;
					switch (inDelete)
					{
					case 1: {DeleteAllCopies(); };
					default:
						ok = false;
						break;
					}
				} break;
				case 4: {
				
					extensions.clear();
				
				} break;
				default:
					ok = false;
					break;
				}


			} while (ok);
		} break;
		case 3:
		{
			std::string prefix;
			std::string extension;
			int nthToBeRemoved = -1;
			bool ok = true;
			do
			{
				std::cout << "Press 1. to change extension of files to be renamed" << std::endl;
				std::cout << "Press 2. to enter new prefix name for files" << std::endl;
				std::cout << "Press 3. to rename with current settings" << std::endl;
				std::cout << "Press 4. to set n-th file to be renamed" << std::endl;
				int input2;
				std::cin >> input2;
				switch (input2)
				{
				case 1:
				{
					std::cout << "Input extension" << std::endl;
					std::cin >> extension;
				}break;
				case 2:
				{
					std::cout << "Input prefix" << std::endl;
					std::cin >> prefix;
				}break;
				case 3:
				{
					ok = false;
					RenameFiles(currentPath, prefix, extension, nthToBeRemoved);
				}break;
				case 4:
				{
					std::cout << "Input which nth file will be removed" << std::endl;
					std::cin >> nthToBeRemoved;
				}break;
				}
			} while (ok);
		}break;



		default: std::cout << "WRONG INPUT!" << std::endl;
		}
	}
}
auto main(int nargs, char**args) -> int
{
	StartProgram(args);
}