// 09_Templates.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <type_traits>

using namespace std;

template <typename T> void helper(T t, true_type)
{
	cout << t << " is an integral !" << endl;
}

template <typename T> void helper(T t, false_type)
{
	cout << t << " is not an integral !" << endl;
}

template <typename T> void write(T t)
{
	helper(t, typename is_integral<T>::type());
}

int main()
{
	write(122);
	write(1.22f);
	write("e");
	return 0; 
}

