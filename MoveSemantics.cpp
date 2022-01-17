// MoveSemantics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


//VarArgs Example:
#include <ostream> 
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


/*motivation
* 
* minimum amount of work to move values from one var to another 
* avoid a deep copy by just pointing nto same memory  
*/

template<typename T> 
class myVector
{
public:
	// copy assignment operator (takes an lvalue)
	myVector& operator=(const myVector& rhs) = default;
	myVector() = default;
	myVector(const myVector&) = default;
	// move assignment, takes an r rvalue through the r-value reference 
	myVector& operator=(myVector&& rhs) = default;
};

myVector<int> createVect()
{
	return myVector<int>{};
}


int main()
{

	std::vector<int> v1{1, 2, 3, 4, 5};
	std::vector<int> v2{};
	v2 = v1; //does a copy 
	v2 = std::move(v1); // does a move, mem doesnt change, just pointers change
	//v1 is still around
	std::cout << v1.size() << std::endl; //will be 0, not pointing to anything after move 

	myVector<int> x, y;

	//uses the lvalue = operator. Causes a copy;
	y = x;
	//If there was only one = operator, would have to do a copy
	//since there is an r-value ref = operator, we can use the r-value ref 
	// does a move 
	y = createVect(); 

	// x is an l-value. Move operation treats x as an R-Value 
	y = std::move(x);
	//x lives on even though its contents have been moved 
	//should not use but could copy or move assign to it again 
	//move does a static_cast, doesnt actually move anything but makes it an r-value reference 
	//r-val ref reps a modifiable obj that are no longer needed 

	/*
	move constructor
	Widget( Widget&& w) = default;
	
	move assignment operator
	Widget& operator=( Widget&& w) = default;
	rule of zero = if you can avoid defining default ops, dont. 
	*/

	/*
	class Widget {
		private:
			int i{0};
			std::string s{};
			int* pi{nullptr};
			unique_ptr<int> upi{};
		public: 
		...
		//need to explicitely include the std::move in the ctor b/c in call stack, the value is an r-val ref
		// but once we enter the actual ctor w is an l-val again because it has a name 
		// noexcept added for performance, i.e. .push_back assumes no errors, if dont promise no erros, pushback
		//will fallback to pre c++11 and do a copy. 
		Widget( Widget&& w) noexcept
			: i (std::move(w.i) )
			, s (std::move(w.s) )
			, pi (std::move(w.pi)
			, upi(std::move(w.upi) //does not require setting to null like pi
			{
				w.pi = nullptr //make sure that two pointers not pointing to the same place
			}

		//default is also noexcept and does the std::moves

		default move operations generated if no copy or destructor user defined 
		default copy operations generated if no move operation user defined  
	*/

}

