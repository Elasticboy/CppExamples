// 09_Templates.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <type_traits>
#include <string>

using namespace std;

template<typename... Ts> void safe_print(const char* str, const Ts&... ts);

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
	
	safe_print("bobob", 123, 122.354, string("qdsf"));
	//getString("bobob", 123.123);
	//getString("bobob", "HELLO");
	//getString("bobob", string("STRING"));

	system("PAUSE");
	return 0; 
}

	/** is_arithmetic */
	template<typename T> typename std::enable_if<std::is_integral<T>::value, long>::type
	normalizeArg(const T& arg) {
		cout << "--is_arithmetic" << endl;
		return arg;
	}
	
	/** is_arithmetic */
	template<typename T> typename std::enable_if<std::is_floating_point<T>::value, double>::type
	normalizeArg(const T& arg) {
		cout << "--is_arithmetic" << endl;
		return arg;
	}

	const char* normalizeArg(const string& arg)
	{
		return arg.c_str();
	}

	/** NOT string => fail /
	template<typename T>
	std::string helper(const T& arg) {
		cout << "FAIL" << endl;
		throw invalid_argument("Translator::getString() does not accept argument type \"" + typeid(T).name() + "\".");
	}*/

	template<typename... Ts> void safe_print(const char* str, const Ts&... ts)
	{
		cout << printf(str, normalizeArg(ts)...) << endl;
	}