// 04_WinAPI.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <string>
#include <regex>
#include <iostream>

#include <Windows.h>

using namespace std;

enum file_type
{ 
	regular_file,
	directory_file
};

struct file_struct {
	string path;
	long long size;
	file_type type;
};
vector<file_struct> list_files(const string& root, bool recursive = false, const string& filter = ".*", bool regularFilesOnly = false);

int main()
{
	try {
		auto files = list_files("c:\\");

		for (const auto& file : files) {
			cout << "Path : "	<< file.path << endl;
			cout << "type : "	<< file.type << " size : "	<< file.size << "o" << endl;
		}
	} catch (const exception& e) {
		cerr << "Error while listing : " << endl;
		cerr << e.what() << endl;
	}

	return 0;
}

const string file_current_element = ".";
const string file_back_element = "..";
const char file_separator = '\\';

bool is_separator(const char& c)
{
	return (c == '/') || (c == '\\');
}

/**
* Remove the caracters to the right of a string if they are filtered by the given function.
* @param str The string to clear.
* @param filter The filter function.
* @return The right cleared string.
*/
const string clear_right(const string& str, bool (*filter)(const char& c))
{
	string trimmedStr = str;
	while(filter(trimmedStr.at(trimmedStr.length() -1))) {
		trimmedStr = trimmedStr.substr(0, trimmedStr.length() -1);
	}
	return trimmedStr; 
}

/**
* List files and directories of the directory passed as parameter.
* @param root The root directory to explore.
* @param recursive True if the search should be recusive. False otherwise.
* @param filter A string that will be turn into a regexp to select special files or directory.
* @return A vector containing the File find by the function.
*/
std::vector<file_struct> list_files(const string& root, bool recursive, const string& filter, bool regularFilesOnly)
{
	// TODO: Implement boost for the rest of the function.

	string clearedRoot = clear_right(root, is_separator) + file_separator;
	// TODO: get full path function if root is relative

	// Prepare string to use FindFile function
	// Add "\\*" to the end of the path.
	const string searchPath = clearedRoot + "*";

	// Check wheather the path is longer than the maximum authorized size (MAX_PATH) 
	if (searchPath.length() > MAX_PATH) {
		throw exception("fs_utils::list_files : Path is too long.");
	}

	// Search for the first file of the directory.
	WIN32_FIND_DATAA fileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	hFind = FindFirstFileA(searchPath.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		throw exception("fs_utils::list_files : Invalid handler value.");
	}

	// List all the files in the directory and get some informations
	const regex regexFilter(filter);
	LARGE_INTEGER filesize;
	auto fileList = vector<file_struct>();

	do {
		file_struct file;
		file.path = clearedRoot + fileData.cFileName;

		// file size in bytes
		filesize.LowPart	= fileData.nFileSizeLow;
		filesize.HighPart	= fileData.nFileSizeHigh;
		file.size = filesize.QuadPart;

		// It is a directory
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			// Skip directories file_current_element and "$RECYCLE.BIN"
			if (strcmp(fileData.cFileName, file_current_element.c_str()) == 0 ||
				strcmp(fileData.cFileName, file_back_element.c_str()) == 0 ||
				strcmp(fileData.cFileName, "$Recycle.Bin") == 0 ||
				strcmp(fileData.cFileName, "$RECYCLE.BIN") == 0 ) {
					continue;
			}
			file.type = file_type::directory_file;

			if (recursive && strcmp(fileData.cFileName, file_back_element.c_str()) != 0) {
				// List the files in the directory
				auto directoryFiles = list_files(file.path, recursive, filter, regularFilesOnly);
				// Add to the end of the current vector
				fileList.insert(fileList.end(), directoryFiles.begin(), directoryFiles.end());
			}

		} else { // If is a file
			if (filter != "" && !regex_match(file.path, regexFilter)) {
				continue;
			}
			file.type = file_type::regular_file;
		}

		if (regularFilesOnly && file.type != file_type::regular_file) {
			continue;
		}

		// Add the file or directory to the list
		fileList.push_back(file);

	} while (FindNextFileA(hFind, &fileData) != 0);

	FindClose(hFind);

	auto error = GetLastError();
	if (error != ERROR_NO_MORE_FILES) {
		const string msg = "list_files, FindNextFile error : " + to_string(error);
		throw exception(msg.c_str());
	}
	return fileList;
}

