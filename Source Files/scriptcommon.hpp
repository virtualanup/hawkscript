/*
scriptcommon.hpp
declaration of common functions and classes used in the hawk script
February 11, 2013

This common file is seperated from common header file of the main engine because the
script itself is reusable as a completely independent unit from the game
engine
*/

#ifndef _HAWK_SCRIPTCOMMON_HPP_
#define _HAWK_SCRIPTCOMMON_HPP_

#include<string>
#include<map>
#include<vector>
#include<iostream>
using namespace std;

namespace hawk
{

#define _HK_DEBUG_e
#ifdef _HK_DEBUG_
#define HK_DEBUGP(x) cout<<x<<endl;
#else
#define HK_DEBUGP(x)  
#endif



	enum TokenId { 
		TOKEN_OPERATOR=0,
		TOKEN_NUM,//numerical value
		TOKEN_STR,//string value
		TOKEN_VARIABLE,
		TOKEN_FUNCTION,
		TOKEN_STATEMENT,
		TOKEN_EOL,
		TOKEN_EOF,
		TOKEN_ERROR
	};


	//enumeration of statements
	enum StatementId{	
		STMT_IF,
		STMT_ELSE,
		STMT_ELSEIF,
		STMT_ENDIF,

		STMT_FOR,
		STMT_TO,
		STMT_STEP,
		STMT_NEXT,

		STMT_WHILE,
		STMT_REPEAT,
		STMT_GOTO,
		STMT_JUMP,
		STMT_RETURN,

		STMT_PRINT,
		STMT_INPUT,

		STMT_REM,
		STMT_END
	};

	enum OperatorId
	{
		OPER_COMMA=0,
		OPER_PLUS,
		OPER_MINUS,
		OPER_MULT,
		OPER_DIVIDE,
		OPER_INT_DIVIDE,
		OPER_AND,
		OPER_OR,
		OPER_NOT,
		OPER_MOD,
		OPER_POW,
		OPER_LPAREN,
		OPER_RPAREN,
		OPER_LT,
		OPER_GT,
		OPER_EQ,
		OPER_LTEQ,
		OPER_GTEQ,
		OPER_NTEQ
	};


	const int TotalOperators=OPER_NTEQ+1;
	const int TotalStatements=STMT_END+1;

	struct Statement
	{
		StatementId ID;
		string Text;
	};

	struct Operator
	{
		OperatorId ID;
		string Symbol;
	};

	extern Operator Operators[TotalOperators];
	extern Statement Statements[TotalStatements];

	//some common functions
	string NumToStr(int no);
	string ToUpper(const string& Source);




	struct Error
	{
		const string ErrorText;
		const int LineNo;
		Error(const string& Errortext, const int& Lineno=-1):ErrorText(Errortext),LineNo(Lineno){}
		~Error(){}
	};

	struct ForData
	{
		string varname;
		double from,till,step;
	};

}
#endif