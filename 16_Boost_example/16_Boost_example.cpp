// 16_Boost_example.cpp : Defines the entry point for the console application.
//

#include <boost\regex.hpp>
#include <boost\filesystem.hpp>
#include <iostream>
#include <string>

int regexExample();
int listFileExample();
int addPath();

int main()
{
	//regexExample();
	//listFileExample();
	int result = addPath();

	system("PAUSE");
	return result;
}

int regexExample()
{
	std::string line;
	boost::regex pattern( "^Subject: (Re: |Aw: )*(.*)" );

	while (std::cin)
	{
		std::getline(std::cin, line);
		boost::smatch matches;
		if (boost::regex_match(line, matches, pattern)) {
			std::cout << matches[2] << std::endl;
		}

		boost::regex r( "^exit$" );
		boost::smatch exitMatches;
		if (boost::regex_match(line, exitMatches, r)) {
			break;
		}
	}
	return EXIT_SUCCESS;
}


int listFileExample()
{
	try {
		const bool recursive = false;
		const std::string root = ".\\test"; 

		namespace fs = boost::filesystem;
		fs::path rootPath(root);

		// Throw exception if path doesn't exist or isn't a directory.
		if (!fs::exists(rootPath)) {
			throw std::exception("rootPath does not exist.");
		}
		if (!fs::is_directory(rootPath)) {
			throw std::exception("rootPath is not a directory.");
		}

		fs::directory_iterator end_itr;
		for( fs::directory_iterator it(rootPath); it != end_itr; ++it) {

			std::cout << it->path()<< std::endl;

			// For a directory
			if (fs::is_directory(it->status())) {
				std::cout << "is_directory" << std::endl;
				if (recursive && it->path().string() != "..") {

				}

			} else if (fs::is_regular_file(it->status())) { // For a regular file
				std::cout << "is_regular_file" << std::endl;
				std::cout << "size : " << fs::file_size(it->path()) << "octets" << std::endl;

			} else {
				std::cout << "is something else !?" << std::endl;
				// TODO: throw an error
			}
		}
		return EXIT_SUCCESS;

	} catch (const boost::filesystem::filesystem_error& e) {
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

int addPath()
{
	auto path1 = boost::filesystem::path("..\\..\\hello");
	auto path2 = boost::filesystem::path("./my/world/");
	
	auto path3 = path1.string() + path2.string();
	std::cout << path3 << std::endl;

	return EXIT_SUCCESS;
}