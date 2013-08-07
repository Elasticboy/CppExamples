// 01_FileIO.cpp : Defines the entry point for the console application.
#include <iostream>

int uninitialized_global;

// !!! Compile time failure : const object must be initialized if not extern
//const int uninitialized_global_const;

int main()
{
	int uninitialized_local;
	// !!! Compile time failure : uninitialized local variable 'uninitialized_local' used
	//std::cout << uninitialized_local << std::endl;
	
	// Undefined behavior but prints a value
	std::cout << uninitialized_global << std::endl;

	// Program does not even reach this point if const not initialized
	//std::cout << uninitialized_global_const << std::endl;

	int y = 10;
	std::cout << y << std::endl;

	system("PAUSE");

	return 0;
}