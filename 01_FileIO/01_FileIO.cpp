// 01_FileIO.cpp : Defines the entry point for the console application.
#include <iostream>
#include <string>

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

	int x(10);
	std::cout << x << std::endl;
	
	std::cout << std::endl;
	std::cout << "References" << std::endl;
	
	int* y(&x); // pointer to x
	int& z(x); // reference to x

	x++;
	std::cout << "x++" << std::endl;
	std::cout << "x : " << x << std::endl;
	std::cout << "*y : " << *y << std::endl;
	std::cout << "z : " << z << std::endl;
	
	std::cout << std::endl;

	(*y)++;
	std::cout << "(*y)++" << std::endl;
	std::cout << "x : " << x << std::endl;
	std::cout << "*y : " << *y << std::endl;
	std::cout << "z : " << z << std::endl;
	
	std::cout << std::endl;

	z++;
	std::cout << "z++" << std::endl;
	std::cout << "x : " << x << std::endl;
	std::cout << "*y : " << *y << std::endl;
	std::cout << "z : " << z << std::endl;

	std::cout << std::endl;

	int w(z); // Copy of the value
	z++;
	std::cout << "z++" << std::endl;
	std::cout << "w : " << w << std::endl;
	std::cout << "x : " << x << std::endl;
	std::cout << "*y : " << *y << std::endl;
	std::cout << "z : " << z << std::endl;
	
	
	std::cout << std::endl << "Pi : ";
	double pi(-1.0);
	std::cin >> pi;

	std::cin.ignore(); // Mandatory before calling getline()

	std::cout << std::endl << "Tell me your name : ";
	std::string name("My Name");
	//std::cin >> name; // cin does not like spaces
	getline(std::cin, name);
	std::cout << "Name : " << name << std::endl;

	system("PAUSE");

	return 0;
}