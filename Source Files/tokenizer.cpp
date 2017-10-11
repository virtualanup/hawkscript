#include"tokenizer.hpp"
namespace hawk
{

	//definitions for tokenizer

	bool Tokenizer::IsSpace(char c) const//determines if the character is space
	{
		return (c==' ' || c=='\t');
	}
	bool Tokenizer::IsLineEndedColon() const
	{
		return IsColonEnd;
	}
	double Tokenizer::GetNumVal() const 
	{
		return NumVal;
	}
	string Tokenizer::GetStrVal() const
	{
		return StrVal;
	}
	StatementId Tokenizer::GetLastStatementment() const 
	{ 
		return LastStatement;
	}
	OperatorId Tokenizer::GetLastOperator() const 
	{
		return LastOperator;
	}
	string Tokenizer::GetLastFunctionName() const 
	{
		return LastFunctionName;
	}
	string Tokenizer::GetLastVarName() const 
	{
		return  LastVarName;
	}


	void Tokenizer::SkipSpace() //skips whitespace.
	{
		char c=GetChar();//get a char
		if(IsSpace(c))
		{
			while(IsSpace(c))//if it is whitespace, pop it and look for another one
				c=PopChar();
			streamp--;
		}
	}


	char Tokenizer::PopChar()//gets input and increase pointer
	{
		if(streamp>=program.size())
			return 0;
		return program[streamp++];
	}

	char Tokenizer::GetChar() const//gets input but doesnot increase the pointer
	{
		if(streamp>=program.size())
			return 0;
		return program[streamp];
	}

	void Tokenizer::SkipHardLine()//skips till \0 or \n is found
	{
		while( GetChar()!=0 && GetChar()!='\n')
			PopChar();
	}

	void Tokenizer::ResetPointer()
	{
		streamp=0;
	}
	int Tokenizer::GetPointer() const
	{
		return streamp;
	}
	void Tokenizer::SetPointer(const int& Point)
	{
		streamp=Point;
	}
	void Tokenizer::SetProgram(const string& Program)
	{
		program=Program;
		streamp=0;//we start from the very first of the program
	}
	TokenId Tokenizer::PeekNextToken()
	{
		int pointer=GetPointer();
		TokenId token=GetNextToken();
		SetPointer(pointer);
		return token;
	}
	TokenId Tokenizer::GetLastToken() const
	{
		return LastToken;
	}
	void Tokenizer::IgnoreToken(TokenId Token)
	{
		//ignores the next token if it is of the given type
		if(PeekNextToken() == Token)
			GetNextToken();
		return;
	}
	bool Tokenizer::IsFinished()
	{
		SkipSpace();
		return GetChar()==0;//returns if the program is finished or not
	}

	TokenId Tokenizer::GetNextToken()
	{
		LastToken=TOKEN_ERROR;//this is error by default.

		SkipSpace();//skip space first
		char c=GetChar();//get the next character
		if(c == 0) //see if we reached the end of the file
		{
			LastToken=TOKEN_EOF;
			HK_DEBUGP("Token EOF");
		}
		else if(c=='\n' || c==';')
		{
			PopChar();
			IsColonEnd=(c==';');
			LastToken=TOKEN_EOL;//end of line
			HK_DEBUGP("Token EOL");
		}
		else if(isdigit(c))//we don't allow 2+.3
		{
			NumVal=0;
			while(isdigit(GetChar()))
				NumVal=NumVal*10+PopChar()-'0';
			//if there is a '.' get the number after it also
			if(GetChar() == '.')
			{
				long a,b;
				PopChar();//pop out the '.'
				a=0,b=1;
				while(isdigit(GetChar()))
				{
					a=a*10+PopChar()-'0';
					b=b*10;
				}
				//change the long integer to decimal
				NumVal=NumVal+(double)a/(double)b;
			}
			if(toupper(GetChar()) == 'E')
			{
				double factor=1;
				long a;
				PopChar();
				a=0;
				if((GetChar()=='-') || (GetChar()=='+'))//unary - or +
				{
					if(PopChar()=='-')
						factor=-1.0;
				}
				while(isdigit(GetChar()))
				{
					a=a*10+PopChar()-'0';
				}
				//change the long integer to decimal
				NumVal=NumVal*pow(10,(double)factor*a);
			}
			LastToken=TOKEN_NUM;
			HK_DEBUGP("Token number");
		}
		else if(c=='"')
		{
			//may be a string character
			StrVal="";
			char c=PopChar();//pop " out
			while( (!(GetChar() == '"' && c!= '\\'))  || GetChar()=='\n' || GetChar() == 0)//when " is followed by \ or when line or file ends
			{
				//TODO \n etc doesnot work currently
				StrVal+=(c=PopChar());
			}
			if(PopChar() != '"')//pop the ending " if it is there
			{
				throw Error(string("Unexpected End of String")+StrVal,-1);
			};
			LastToken=TOKEN_STR;
			HK_DEBUGP("Token String "+StrVal);
		}
		else if(isalpha(c) || c == '_')
		{
			//it can be statement, function or variable name (or operator like not, and, or, etc)
			string name;
			name+=toupper(PopChar());
			while(isalnum(GetChar()) || GetChar()=='_')//all combination of alphanumeric and underscore
				name+=toupper(PopChar());

			bool Found=false;
			//now check if it is statement
			for(int i=0;i<TotalStatements;i++)
			{
				if(Statements[i].Text == name)
				{
					//statement found
					LastStatement=Statements[i].ID;
					HK_DEBUGP(name+" statement found");
					Found=true;
					LastToken=TOKEN_STATEMENT;
					break;
				}
			}

			//if not found, search for function
			if(!Found)
			{
				if(functionhandler.FunctionExists(name))
				{
					LastFunctionName=name;
					HK_DEBUGP(name+" function found");
					Found=true;
					LastToken=TOKEN_FUNCTION;
				}
			}
			if(!Found)
			{
				//it may be a operator with string name
				int operatorlen;
				//try to match it with an operator
				for(int i=0;i<TotalOperators;i++)
				{
					if(Operators[i].Symbol == name)
					{
						LastOperator=Operators[i].ID;
						HK_DEBUGP(Operators[i].Symbol+" operator found");
						LastToken=TOKEN_OPERATOR;
						Found=true;
						break;
					}
				}
			}
			//if it not function or statement, it is a variable name
			if(!Found)
			{
				LastVarName=name;
				LastToken=TOKEN_VARIABLE;
				HK_DEBUGP(name+" variable found");
			}
		}
		else
		{
			//cout<<"Checking "<<c<<" for operator"<<endl;
			int operatorlen;
			//try to match it with an operator
			for(int i=0;i<TotalOperators;i++)
			{
				//make sure that we will not reach EOF before completing comparing with the operator
				operatorlen= Operators[i].Symbol.size();
				if(streamp+operatorlen <= program.size())
				{
					//compare the string
					//cout<<program.substr(streamp,operatorlen)<<"-"<< Operators[i].Symbol << (program.substr(streamp,operatorlen) == Operators[i].Symbol) <<endl;
					if(program.substr(streamp,operatorlen) == Operators[i].Symbol)
					{
						LastOperator=Operators[i].ID;
						HK_DEBUGP(Operators[i].Symbol+" operator found");
						LastToken=TOKEN_OPERATOR;
						streamp+=operatorlen;
						break;
					}
				}
			}

		}

		if(LastToken == TOKEN_ERROR)
		{
			//check if it is an operator

#ifdef _HK_DEBUG_
			cout<<"\n\nError creating token "<<c<<endl;
#endif

		}

		return LastToken;

	}
}