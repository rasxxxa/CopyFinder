module;
export module CopyFinderModule;

import <iostream>;
import <filesystem>;
import <vector>;
import <concepts>;
import <unordered_map>;
import <set>;
import <algorithm>;
import <numeric>;
import <fstream>;
import <list>;

export struct FileInfo
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


std::string GetFileExtension(const std::string& file)
{
	long long pos = file.find_last_of('.');
	if (pos <= 0 || pos >= std::numeric_limits<size_t>::max())
		pos = 0;

	return file.substr(pos, file.size() - pos);
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
		if (!files.empty())
			return;

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

	void ListAllFiles(bool recursive = false, const std::string_view& path = ".")
	{ 
		if (files.empty())
			LoadAllFiles(recursive, path);
	}

	void ListAllDuplicates(bool recursive = false, const std::string_view& path = ".")
	{
		if (files.empty())
			LoadAllFiles(recursive, path);

		for (const auto& elems : files)
		{
			auto extension = elems.first;
			const auto& elements = elems.second;
			std::list<FileInfo> filesWithInfos;
			std::ranges::for_each(elements, [&](const auto& elem) {filesWithInfos.push_back(elem); });
			for (auto listIterator = filesWithInfos.begin(); listIterator != filesWithInfos.end(); listIterator++)
			{
				std::cout << "Copies of file" << (*listIterator).filePath << std::endl;
				std::vector<std::string> copies;
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
						copies.push_back((*innerIt).filePath);
						innerIt = filesWithInfos.erase(innerIt);
					}
					else
					{
						innerIt++;
					}

				}

				for (auto it : copies)
					std::cout << "\t" << it << std::endl;

			}
		}



	}

}