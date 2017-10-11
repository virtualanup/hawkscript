///*
//variable.hpp
//declaration of the variable class for the hawk script
//February 18 2013
//*/
#include "Variable.hpp"
#include<cmath>

namespace hawk
{
	void Variable::SetVal(const string& Val)
	{
		type=VAR_STR;
		StrVal=Val;
	}
	void Variable::SetVal(double Val)
	{
		type=VAR_NUM;
		NumVal=Val;
	}
	int Variable::GetIntVal()
	{
		return static_cast<int>(NumVal);
	}
	string Variable::GetStrVal()
	{
		return StrVal;
	}
	double Variable::GetNumVal()
	{
		return NumVal;
	}

	void VarManager::ClearAllVars()
	{
		VarStore.clear();
	}
	void VarManager::ClearAllConst()
	{
		Constants.clear();
	}
	void VarManager::AddVariable(const string& name, Variable val)
	{
		string n=ToUpper(name);
		//don't let assignment in constants
		if(Constants.count(n)>0)
			throw string("Can't assign to constant "+n);
		VarStore[n]=val;
	}

	void VarManager::AddConstant(const string& name, Variable val)
	{
		string n=ToUpper(name);
		//if the constant alreay exists in variable list, remove it
		if(VarStore.count(n)>0)
			VarStore.erase(n);
		Constants[n]=val;
	}

	Variable VarManager::GetVariable(const string& name)
	{
		string n=ToUpper(name);
		//search in variable list first
		if(VarStore.count(n)>0)
			return VarStore[n];
		//search in constant list
		if(Constants.count(n) > 0)
			return Constants[n];
		throw string("Undefined variable or constant "+n);
	}
	void VarManager::LoadDefaultConstants()
	{
		AddConstant("PI",3.14159265358979323846);
	}
}