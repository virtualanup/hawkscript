/*
functions.h
declaration of the function class,  function handler class and other classes of hawk script
February 11, 2013
*/
#ifndef _HAWK_FUNCTIONS_HPP_
#define _HAWK_FUNCTIONS_HPP_

#include"scriptcommon.hpp"
#include"variable.hpp"
#include<ctime>

namespace hawk
{
	typedef Variable (*FuncPointer)(vector<const Variable>);

	/*
	this struct is used to store a function. Run is the pointer to the function that accepts a vector of variable and returs a variable
	*/
	struct Function
	{
		string Name;
		FuncPointer Run;
		int minparam,maxparam;
		string datatypes;
		Function(string n,FuncPointer p,int minp,int maxp,string data=""):Name(n),Run(p),minparam(minp),maxparam(maxp),datatypes(data)
		{
		}
		Function(){}
		virtual ~Function(){}
	};

	/*
	the functionhandler stores the functions and their name, searches for functions, calls them , adds new function etc
	*/
	class FunctionHandler
	{
		int FunctionCount;
		map<string,Function> Functions;
	public:
		void AddFunction(Function Function);
		Variable RunFunction(string FuncName,vector<const Variable>&);
		void RemoveFunction(Function Function);
		void LoadDefaults();//load the default functions
		int GetFunctionCount() const;
		bool FunctionExists(const string& Name) const;
	};

}
#endif
