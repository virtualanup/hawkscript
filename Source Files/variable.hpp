/*
variable.hpp
declaration of the variable class for the hawk script
February 18 2013
*/
#ifndef _HAWK_VARIABLE_HPP_
#define _HAWK_VARIABLE_HPP_
#include"scriptcommon.hpp"

namespace hawk
{
	//can we make this union?
	class Variable
	{
	public:
		enum Type 
		{
			VAR_NUM,//number
			VAR_STR,//string
		};

		string StrVal;
		double NumVal;

		Type type;//type of variable
		
		Variable(){}
		Variable(const Variable& copy)
		{
			StrVal=copy.StrVal;
			NumVal=copy.NumVal;
			type=copy.type;
		}
		Variable(double Val):NumVal(Val),type(VAR_NUM){}
		Variable(const string& Val):StrVal(Val),type(VAR_STR){}
		void SetVal(const string& Val);
		void SetVal(double Val);

		int GetIntVal();//since we require to convert double value to integer a lot, this function is handy
		string GetStrVal();
		double GetNumVal();

	};

	class VarManager
	{
		map<string,Variable> VarStore;
		map<string,Variable> Constants;
		int varcount;
		int constcount;
	public:
		void AddVariable(const string& name, Variable val);
		void AddConstant(const string& name, Variable val);
		Variable GetVariable(const string& name);
		void LoadDefaultConstants();
		void ClearAllVars();
		void ClearAllConst();
	};
}
#endif