/*
tokenizer.hpp
declaration of the tokenizer class for the hawk script
January, 2013

*/
#ifndef _HAWK_TOKENIZER_HPP_
#define _HAWK_TOKENIZER_HPP_

#include"scriptcommon.hpp"
#include"functions.hpp"
namespace hawk
{
	class Tokenizer
	{
		//we store the program as a map because we need the line numbers
		string program;
		
		int streamp;//the pointer to the currently reached steam of the source code
		double NumVal;
		string StrVal;

		TokenId LastToken;
		StatementId LastStatement;//last statement
		OperatorId LastOperator;//last operator


		string LastFunctionName;
		string LastVarName;//name of the variable
		
		bool IsColonEnd;//was the last line ended using a semi colon (;)
		FunctionHandler& functionhandler;
	protected:
		bool IsSpace(char c) const;
		void SkipSpace();
		int GetInteger();
	public:
		Tokenizer(FunctionHandler& handler):functionhandler(handler){}

		~Tokenizer(){}

		bool IsLineEndedColon() const;
		double GetNumVal() const;
		string GetStrVal() const;
		StatementId GetLastStatementment() const;
		OperatorId GetLastOperator() const;
		string GetLastFunctionName() const;
		string GetLastVarName() const;

		char PopChar();
		char GetChar() const;
		void SkipHardLine();

		void ResetPointer();
		int GetPointer() const;
		void SetPointer(const int& Point);
		void SetProgram(const string& Program);
		//parses the next token and and returns its type
		TokenId GetNextToken();
		TokenId PeekNextToken();
		TokenId GetLastToken() const;
		void IgnoreToken(TokenId Token);
		bool IsFinished();
	};
}
#endif