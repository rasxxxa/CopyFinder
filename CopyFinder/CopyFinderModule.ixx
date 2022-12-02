module;

export module CopyFinderModule;

export import <iostream>;
export import <filesystem>;
import <vector>;
import <concepts>;
export import <unordered_map>;
export import <set>;
import <algorithm>;
import <numeric>;
import <fstream>;
import <list>;
import <thread>;

struct FileInfo
{
	long long fileSize;
	std::string filePath;
	constexpr bool operator < (const FileInfo& other) const
	{
		if (other.fileSize == fileSize)
			return std::equal(filePath.begin(), filePath.end(), other.filePath.begin(), other.filePath.end());
		else
			return fileSize < other.fileSize;
	}

	constexpr bool operator == (const FileInfo& other) const
	{
		if (other.fileSize == fileSize)
			return std::equal(filePath.begin(), filePath.end(), other.filePath.begin(), other.filePath.end());
		else
			return false;
	}
};

// extension key - FileInfo data
std::unordered_map<std::string, std::multiset<FileInfo>> files;
std::vector<std::string> copiesForDelete;
std::vector<std::thread> threadsForDuplicates;


std::string GetFileExtension(const std::string& file)
{
	long long pos = file.find_last_of('.');
	if (pos <= 0 || pos >= std::numeric_limits<int>::max())
		pos = 0;

	return file.substr(pos, file.size() - pos);
}

bool DeleteFile(const std::string& file)
{
	if (std::filesystem::exists(file))
	{
		remove(file.c_str());
		return true;
	}
	return false;
}



bool CompareFile(const std::string& file1, const std::string& file2) 
{
	std::ifstream f1(file1, std::ifstream::binary);
	std::ifstream f2(file2, std::ifstream::binary);

	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
		std::istreambuf_iterator<char>(),
		std::istreambuf_iterator<char>(f2.rdbuf()));
}


void PrintFiles()
{
	std::vector<std::string> filesElements;
	for (const auto& extension : files)
	{
		std::cout << "With extension: " << extension.first << std::endl;
		for (const auto& fileInfos : extension.second)
		{
			filesElements.push_back(fileInfos.filePath);
			std::cout << "File: " << fileInfos.filePath << std::endl;
			std::cout << "Size: " << fileInfos.fileSize << std::endl;
		}
	}
	std::cout << std::boolalpha;
	std::cout << CompareFile(filesElements[0], filesElements[1]);
}


void ThreadHelper(const std::string& extension, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>& copiesOfElems)
{
	const auto& elements = files[extension];
	std::list<FileInfo> filesWithInfos;
	std::ranges::for_each(elements, [&](const auto& elem) {filesWithInfos.push_back(elem); });
	copiesOfElems[extension] = std::unordered_map<std::string, std::vector<std::string>>();
	for (auto listIterator = filesWithInfos.begin(); listIterator != filesWithInfos.end(); listIterator++)
	{
		copiesOfElems[extension][(*listIterator).filePath] = std::vector<std::string>();
		for (auto innerIt = listIterator; innerIt != filesWithInfos.end();)
		{
			if (*listIterator == *innerIt)
			{
				innerIt++;
				continue;
			}

			if ((*listIterator).fileSize != (*innerIt).fileSize)
				break;

			if (CompareFile((*listIterator).filePath, (*innerIt).filePath))
			{
				copiesOfElems[extension][(*listIterator).filePath].push_back((*innerIt).filePath);
				innerIt = filesWithInfos.erase(innerIt);
			}
			else
			{
				innerIt++;
			}

		}

	}
}

using namespace std::filesystem;


template <typename T>
concept DirectoryEntry = std::same_as<T, directory_entry>;

template <typename T>
requires DirectoryEntry<T>
void AddFileToListOfFiles(const T& fileIt)
{
	if (!fileIt.is_directory())
	{
		FileInfo info;
		info.filePath = fileIt.path().string();
		info.fileSize = fileIt.file_size();
		auto extension = GetFileExtension(info.filePath);
		if (files.count(extension))
		{
			files[extension].insert(info);
		}
		else
		{
			std::multiset<FileInfo> newFilesSet;
			newFilesSet.insert(info);
			files[extension] = newFilesSet;
		}
	}
}


export
{
	void LoadAllFiles(bool recursive, const std::string_view& path = ".")
	{
		files.clear();

		if (recursive)
		{
			for (const auto& fileIt : recursive_directory_iterator(path))
			{
				AddFileToListOfFiles(fileIt);
			}
		}
		else
		{
			for (const auto& fileIt : directory_iterator(path))
			{
				AddFileToListOfFiles(fileIt);
			}
		}
	}

	void ListAllFiles(bool recursive = false, const std::string& path = ".")
	{ 
		if (files.empty())
			LoadAllFiles(recursive, path);
	}

	void ListAllDuplicates(const std::unordered_map<std::string, bool>& extensions, bool recursive = false, const std::string& path = ".")
	{
		threadsForDuplicates.clear();
		copiesForDelete.clear();
		LoadAllFiles(recursive, path);

		// too complex
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> copiesOfElems;

		std::erase_if(files, [extensions](const auto& fileValue) 
			{
				return extensions.count(fileValue.first) == 0;
			});

		
		for (const auto& elems : files)
		{
			threadsForDuplicates.emplace_back(&ThreadHelper, elems.first, std::ref(copiesOfElems));
		}

		for (auto& thread : threadsForDuplicates)
			thread.join();

		for (const auto& file : copiesOfElems)
		{
			std::ofstream out;
			std::string outputSource = path;
			outputSource.append("\\");
			outputSource.append(file.first);
			outputSource.append("_copies.txt");
			out.open(outputSource.c_str());

			auto mapOfFilesOfExtension = file.second;

			for (const auto& copies : mapOfFilesOfExtension)
			{
				if (!copies.second.empty())
				{
					out << "File: " << copies.first << " has copies:" << std::endl;
					unsigned long long copyNumber = 1;
					for (const auto& copy : copies.second)
					{
						copiesForDelete.push_back(copy);
						out << "\t" << std::to_string(copyNumber++) << ": " << copy << std::endl;
					}
				}
			}


			out.close();
		}

	}

	void DeleteAllCopies()
	{
		unsigned long deleted = 0;
		for (const auto& path : copiesForDelete)
			if (DeleteFile(path))
				deleted++;

		std::cout << "Number of files deleted: " << deleted << std::endl;
	}

	void RenameFiles(const std::string& path, const std::string& prefix, const std::string& extension = "", const int nthToBeRemoved = -1)
	{
		unsigned long fileNumber = 1;
		unsigned long fileCounter = 1;
		for (const auto& pathIt : directory_iterator(path))
		{
			if (!std::filesystem::is_directory(pathIt))
			{
				auto extensionInner = GetFileExtension(pathIt.path().string());
				if (!extension.empty())
				{
					if (extensionInner != extension)
						continue;
				}
				fileCounter++;
				if (nthToBeRemoved > 0 && fileCounter % nthToBeRemoved == 0)
				{
					remove(pathIt.path().c_str());
					continue;
				}

				auto newName = path;
				newName.append("\\");
				newName.append(prefix);
				newName.append(std::to_string(fileNumber++));
				newName.append(extensionInner);
				rename(pathIt.path().c_str(), newName.c_str());
			}
		}

	}

	void SearchFiles(const std::string& prefix, const std::string& infix, const std::string& sufix)
	{

	}


}