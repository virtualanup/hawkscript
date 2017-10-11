/*
script.hpp
declaration of the main script class
January 2013
*/
#ifndef _HAWK_SCRIPT_HPP_
#define _HAWK_SCRIPT_HPP_
#include"scriptcommon.hpp"
#include"tokenizer.hpp"
#include"functions.hpp"
#include"variable.hpp"

namespace hawk
{
	class Script
	{
		FunctionHandler functionhandler;
		VarManager varmanager;
		Tokenizer Tokenizer;
		map<int,int> LineNumbers;//index point to line number and the value points to the pointer in the input stream


		vector<int> GosubStack;
		vector<string> ForVarStack;
		vector<ForData> ForDataStack;
		int IfStack,WhileStack,ForStack;
		bool IsRunning;

		Variable Level1();
		Variable Level2();
		Variable Level3();
		Variable Level4();
		Variable Level5();
		Variable Level6();
		Variable Level7();
		Variable Leveltop();

		void NumNumMatch(const Variable& v1,const Variable v2,const string& Operator);
		void StrStrMatch(const Variable& v1,const Variable v2,const string& Operator);
		void SameTypeMatch(const Variable& v1,const Variable v2,const string& Operator);
		bool RunLine(bool Skip);
		//skip parameter tells if the block is to be skipped
		//like for some block inside 'if' if the condition is false
		void RunBlock(bool Skip);
		void SkipLine();
		//the next token must be EOL statement
		void CheckEndOfLine(string after);
	public:
		Script();
		void SetProgram(const string& Program);
		//runs the entire code
		void Run();
		//runs only one line of code
		void Runline(const string& Line);
		//resets the interpreter
		void Reset(bool clearvar=true,bool clearconst=false);
		Variable GetValue(const string& varname);
		double GetNumValue(const string& varname);
		string GetStrValue(const string& varname);
	};
}
#endif