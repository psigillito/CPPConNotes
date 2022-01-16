#include <ostream> 
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
/*******VarArgs*********/
void printListOfInts(std::ostream& out, size_t n, ...)
{
	va_list args;
	va_start(args, n);
	while (n--)
	{
		out << va_arg(args, int) << ", ";
	}
};

void print_my_vals(size_t n, ...)
{
	va_list args;
	va_start(args, n);
	while (n--)
	{
		std::cout << va_arg(args, int) << "! ";
	}
}


/******Variadic Templates********/


// Helper function that uses a template. 
// Pass in value and get back string 
template<typename T>
std::string to_string_impl(const T& arg)
{
	//use stringstram and ostream operator
	//all built-in types have osteam operator so they will work with << 
	std::stringstream ss;
	ss << arg;
	return ss.str();
}


//include an empty overload for when we get to end of args
std::vector<std::string> to_string()
{
	return {};
}

// Variadic Template, specify first argument (so always at least 1 arg)
// then make the second template arg a vararg. 
//this is very inefficient because of the recursion
template<typename T1, typename ... T>
std::vector<std::string> to_string(const T1& t1, const T& ... t)
{
	std::vector<std::string> s;

	//convert the first arg 
	s.push_back(to_string_impl(t1));

	//do a recursive call on the vararg 
	//compiler will auto expand this bc of ...
	const auto remainder = to_string(t ...);
	s.insert(s.end(), remainder.begin(), remainder.end());
	return s;
}

template<typename ... T>
std::vector<std::string> to_string_efficient(const T& ... t)
{
	return { to_string_impl(t)... };
}

int main()
{
	//varArg Example 
	//unsafe, have to spec type and number 
	std::filebuf buf;
	buf.open("test.txt", std::ios::out);
	std::ostream os(&buf);
	printListOfInts(os, 3, 1, 2, 3);
	print_my_vals(3, 6, 6, 6);
	buf.close();

	const auto vec = to_string("hello", true, 6.66, 9);
	const auto eff_vec = to_string_efficient("hello", true, 6.66, 9);

	for (const auto& i : eff_vec)
	{
		std::cout << i << std::endl;
	}


}

