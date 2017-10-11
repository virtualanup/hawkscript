#include "functions.hpp"

namespace hawk
{

	//definitions of the functions that we use in the scripting language
	//randomization functions
	Variable hawkf_rand(vector<const Variable> params)
	{
		int from=0;
		if(params.size()>0)
		{
			from=params[0].GetIntVal();
		}
		int till=INT_MAX-from+1;
		if(params.size() > 1)
		{
			till=params[1].GetIntVal()-from+1;
		}
		int random=rand()%till+from;
		return Variable(random);
	}


	//trigonometric functions
	Variable hawkf_sin(vector<const Variable> params)
	{
		return Variable(sin(params[0].GetNumVal()));
	}
	Variable hawkf_cos(vector<const Variable> params)
	{
		return Variable(cos(params[0].GetNumVal()));
	}

	Variable hawkf_tan(vector<const Variable> params)
	{
		return Variable(tan(params[0].GetNumVal()));
	}

	Variable hawkf_asin(vector<const Variable> params)
	{
		return Variable(asin(params[0].GetNumVal()));
	}

	Variable hawkf_acos(vector<const Variable> params)
	{
		return Variable(acos(params[0].GetNumVal()));
	}

	Variable hawkf_atan(vector<const Variable> params)
	{
		return Variable(atan(params[0].GetNumVal()));
	}

	Variable hawkf_atan2(vector<const Variable> params)
	{
		return Variable(atan2(params[0].GetNumVal(),params[1].GetNumVal()));
	}


	Variable hawkf_sinh(vector<const Variable> params)
	{
		return Variable(sinh(params[0].GetNumVal()));
	}
	Variable hawkf_cosh(vector<const Variable> params)
	{
		return Variable(cosh(params[0].GetNumVal()));
	}
	Variable hawkf_tanh(vector<const Variable> params)
	{
		return Variable(tanh(params[0].GetNumVal()));
	}

	//some logarithmic functions
	Variable hawkf_log10(vector<const Variable> params)
	{
		return Variable(log10(params[0].GetNumVal()));
	}
	Variable hawkf_log(vector<const Variable> params)
	{
		return Variable(log(params[0].GetNumVal()));
	}
	Variable hawkf_exp(vector<const Variable> params)
	{
		return Variable(exp(params[0].GetNumVal()));
	}
	Variable hawkf_sqrt(vector<const Variable> params)
	{
		return Variable(sqrt(params[0].GetNumVal()));
	}
	//other mathematical functions
	Variable hawkf_ceil(vector<const Variable> params)
	{
		return Variable(ceil(params[0].GetNumVal()));
	}
	Variable hawkf_floor(vector<const Variable> params)
	{
		return Variable(floor(params[0].GetNumVal()));
	}
	Variable hawkf_abs(vector<const Variable> params)
	{
		return Variable(fabs(params[0].GetNumVal()));
	}
	Variable hawkf_num(vector<const Variable> params)
	{
		if(params[0].type == Variable::Type::VAR_NUM)
			return Variable(params[0].GetNumVal());
		//convert the string to number
		return Variable(atof(params[0].GetStrVal().c_str()));
	}

	//String functions
	Variable hawkf_str(vector<const Variable> params)
	{
		if(params[0].type == Variable::Type::VAR_STR)
			return Variable(params[0].GetStrVal());
		//convert the number to a string
		char buf[100];
		sprintf(buf,"%.15g",params[0].GetNumVal());
		return Variable(string(buf));
	}

	//String functions
	Variable hawkf_uppercase(vector<const Variable> params)
	{
		string transformed;
		string& from=params[0].GetStrVal();
		for(int i=0;i<from.size();i++)
			transformed+=toupper(from[i]);
		return Variable(transformed);
	}
	Variable hawkf_lowercase(vector<const Variable> params)
	{
		string transformed;
		string& from=params[0].GetStrVal();
		for(int i=0;i<from.size();i++)
			transformed+=tolower(from[i]);
		return Variable(transformed);
	}
	Variable hawkf_ascii(vector<const Variable> params)
	{
		return Variable(static_cast<double>(params[0].GetStrVal()[0]));
	}

	Variable hawkf_len(vector<const Variable> params)
	{
		return Variable(static_cast<double>(params[0].GetStrVal().size()));
	}

	Variable hawkf_substr(vector<const Variable> params)
	{
		const string& Str=params[0].GetStrVal();
		int from=params[1].GetIntVal();
		int count=params[2].GetIntVal();

		from=(from<0)?0:from;
		count=(count<0)?0:count;

		if(from > Str.size())
			from=Str.size();
		//we handle the bound outself. It it is out of bound, we control it
		if(from+count > Str.size())
			count=Str.size()-from;

		return Variable(Str.substr(from,count));
	}
	Variable hawkf_index(vector<const Variable> params)
	{
		//returns the ith character of a string
		//if array out of bound, returns last character
		int index=params[1].GetIntVal();
		if(index > params[0].GetStrVal().size()-1)
		{
			index=params[0].GetStrVal().size()-1;
		}
		string ret;
		ret+=params[0].GetStrVal()[index];
		return Variable(ret);
	}

	void FunctionHandler::LoadDefaults()
	{
		AddFunction(Function("RANDOM",hawkf_rand,0,2,"nn"));
		AddFunction(Function("SIN",hawkf_sin,1,1,"n"));
		AddFunction(Function("COS",hawkf_cos,1,1,"n"));
		AddFunction(Function("TAN",hawkf_tan,1,1,"n"));
		AddFunction(Function("ASIN",hawkf_asin,1,1,"n"));
		AddFunction(Function("ACOS",hawkf_acos,1,1,"n"));
		AddFunction(Function("ATAN",hawkf_atan,1,1,"n"));
		AddFunction(Function("ATAN2",hawkf_atan,2,2,"nn"));
		AddFunction(Function("SINH",hawkf_sinh,1,1,"n"));
		AddFunction(Function("COSH",hawkf_cosh,1,1,"n"));
		AddFunction(Function("TANH",hawkf_tanh,1,1,"n"));

		AddFunction(Function("LOG10",hawkf_log10,1,1,"n"));
		AddFunction(Function("LOG",hawkf_log,1,1,"n"));
		AddFunction(Function("EXP",hawkf_exp,1,1,"n"));
		AddFunction(Function("SQRT",hawkf_sqrt,1,1,"n"));

		AddFunction(Function("CEIL",hawkf_ceil,1,1,"n"));
		AddFunction(Function("FLOOR",hawkf_floor,1,1,"n"));
		AddFunction(Function("ABS",hawkf_abs,1,1,"n"));
		AddFunction(Function("NUM",hawkf_num,1,1,"a"));

		AddFunction(Function("STR",hawkf_str,1,1,"a"));
		AddFunction(Function("UPPERCASE",hawkf_uppercase,1,1,"s"));
		AddFunction(Function("LOWERCASE",hawkf_lowercase,1,1,"s"));
		AddFunction(Function("ASCII",hawkf_ascii,1,1,"s"));
		AddFunction(Function("LEN",hawkf_len,1,1,"s"));
		AddFunction(Function("SUBSTR",hawkf_substr,3,3,"snn"));
		AddFunction(Function("INDEX",hawkf_index,2,2,"sn"));
	}


	void FunctionHandler::AddFunction(Function Function)
	{
		if(FunctionExists(Function.Name))
		{
			throw string("Function "+Function.Name+" already exists!");
		}
		Functions[Function.Name]=Function;
		FunctionCount++;
	}


	Variable FunctionHandler::RunFunction(string Name,vector<const Variable>& Params)
	{
		//make sure that the function exists
		if(! FunctionExists(Name))
			throw "Function "+Name+" doesnot exist!";

		Function& Func=Functions[Name];
		int paramcount=Params.size();

		if(Func.minparam != -1 && paramcount < Func.minparam)
		{
			throw Name+" expects atleast "+NumToStr(Func.minparam)+" parameters, "+NumToStr(paramcount)+" given.";
		}
		else if(Func.maxparam != -1 && paramcount > Func.maxparam)
		{
			throw Name+" expects maximum "+NumToStr(Func.maxparam)+" parameters, "+NumToStr(paramcount)+" given.";
		}
		//make sure that the parameters are of proper data type
		//1 means number, 2 means string and 3 means any of them
		for(int i=0;i<Params.size();i++)
		{
			char cdt;
			if(Func.datatypes.size() > i)
				cdt=Func.datatypes[i];
			else
				cdt=Func.datatypes[Func.datatypes.size()-1];//the last data type is the default
			if(cdt=='n' && Params[i].type != Variable::Type::VAR_NUM)
			{
				throw Name+" expects parameter "+NumToStr(i+1)+" to be Number. String given.";
			}
			else if(cdt=='s' && Params[i].type != Variable::Type::VAR_STR)
			{
				throw Name+" expects parameter "+NumToStr(i+1)+" to be String. Number given.";
			}
		}
		//run the function
		return Func.Run(Params);
	}
	int FunctionHandler::GetFunctionCount() const
	{
		return FunctionCount;
	}
	bool FunctionHandler::FunctionExists(const string& Name) const
	{
		return Functions.count(Name) > 0;
	}

	void FunctionHandler::RemoveFunction(Function Function)
	{
		if(Functions.count(Function.Name) == 0)
		{
			throw string("Function "+Function.Name+" doesnot exist!");
		}
		Functions.erase(Function.Name);
		FunctionCount--;
	}
}