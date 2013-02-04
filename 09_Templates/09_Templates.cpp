// 09_Templates.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <type_traits>
#include <string>

using namespace std;

template <typename T> void helperIntegral(T t, true_type)
{
	cout << t << " is an integral !" << endl;
}

template <typename T> void helperIntegral(T t, false_type)
{
	cout << t << " is NOT an integral !" << endl;
}

template <typename T> void testIntegral(T t)
{
	helperIntegral(t, typename is_integral<T>::type());
}

////////////////////////////////
template <typename T> void testArithmeticOrLiteral(const T& t)
{
	if (is_arithmetic<T>::type()) {
		cout << "is_arithmetic" << endl;
	} else if (is_literal_type<T>::type()) {
		cout << "is_literal_type" << endl;
	} else if (is_same<T, string>::type()) {
		cout << "is_string" << endl;
	} else {
		cout << "Wrong argument : " << typeid(T).name() << endl;
	}
}

int main()
{
	// Test templates
	testIntegral(122);
	testIntegral(1.22f);
	testIntegral("e");
    cout << endl; 

	const int const_int(10);
	int test_int(8);
	const float const_float(12.0f);
	float test_float(12.0f);

	testArithmeticOrLiteral(const_int);
	testArithmeticOrLiteral(test_int);
	testArithmeticOrLiteral(const_float);
	testArithmeticOrLiteral(test_float);
	testArithmeticOrLiteral("bob");
	testArithmeticOrLiteral(L"bob");
	testArithmeticOrLiteral(string("bob"));
    cout << endl; 

	// Test type_traits
	cout << boolalpha;
    cout << is_arithmetic<int>::value << endl;
    cout << is_arithmetic<int&>::value << endl;
    cout << is_arithmetic<int*>::value << endl;
    cout << is_arithmetic<float>::value << endl;
    cout << is_arithmetic<float&>::value << endl;
    cout << is_arithmetic<float*>::value << endl;
	
    cout << endl; 

	system("PAUSE");
	return 0; 
}

