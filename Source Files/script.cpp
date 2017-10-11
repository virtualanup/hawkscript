#include"script.hpp"

namespace hawk
{

	Statement Statements[TotalStatements]=
	{
		{ STMT_IF,"IF"},
		{ STMT_ELSE,"ELSE"},
		{ STMT_ELSEIF,"ELIF"},
		{ STMT_ENDIF,"ENDIF"},

		{ STMT_FOR,"FOR"},
		{ STMT_TO,"TO"},
		{ STMT_STEP,"STEP"},
		{ STMT_NEXT,"NEXT"},

		{ STMT_WHILE,"WHILE"},
		{ STMT_REPEAT,"REPEAT"},

		{ STMT_PRINT,"PRINT"},
		{ STMT_INPUT,"INPUT"},

		{ STMT_GOTO,"GOTO"},
		{ STMT_JUMP,"JUMP"},
		{ STMT_RETURN,"RETURN"},

		{ STMT_REM,"REM"},
		{ STMT_END,"END"}
	};


	Operator Operators[TotalOperators]={
		{ OPER_AND,"AND"},
		{ OPER_OR,"OR"},
		{ OPER_NOT,"NOT"},
		{ OPER_LTEQ,"<="},
		{ OPER_GTEQ,">="},
		{ OPER_NTEQ,"<>"},
		{ OPER_COMMA,","},
		{ OPER_PLUS,"+"},
		{ OPER_MINUS,"-"},
		{ OPER_MULT,"*"},
		{ OPER_DIVIDE,"/"},
		{ OPER_INT_DIVIDE,"\\"},
		{ OPER_POW,"^"},
		{ OPER_LPAREN,"("},
		{ OPER_RPAREN,")"},
		{ OPER_LT,"<"},
		{ OPER_GT,">"},
		{ OPER_EQ,"="},
		{ OPER_MOD,"%"}
	};



	//definitions of script

	Script::Script():Tokenizer(functionhandler)
	{
		functionhandler.LoadDefaults();//load the default functions now
		varmanager.LoadDefaultConstants();
		srand(time(0));
	}

	void Script::SetProgram(const string& Program)
	{
#ifdef _HK_DEBUG_
		cout<<"Got program "<<Program<<endl<<endl;
#endif
		Tokenizer.SetProgram(Program);
		//we then run through the whole program and capture the line numbers
		int curline=1;
		while(!Tokenizer.IsFinished())
		{
			if(Tokenizer.GetNextToken() == TOKEN_NUM)
			{
				//line number detected
				//line number must be followed by space
				if(Tokenizer.GetChar() != ' ')
					throw Error("Expected space after line number",curline);
				//get the line number and store it
				int lineno=Tokenizer.GetNumVal();
				int position=Tokenizer.GetPointer();

				if(LineNumbers.count(lineno) >0)
				{
					throw Error("Duplicate Line Number "+NumToStr(lineno),curline);
				}
				LineNumbers[lineno]=position;
				HK_DEBUGP("Got Line number");
			}
			if(Tokenizer.GetLastToken() != TOKEN_EOL)
			{
				Tokenizer.SkipHardLine();//skip until we find \n or 0
				Tokenizer.GetNextToken();//this will skip the \n character also
			}
			curline++;

		}
		//cout<<"total "<<LineNumbers.size()<<" line numbers";
	}
	void Script::Run()
	{
		try
		{
			//reset the tokenizer
			Tokenizer.ResetPointer();
			GosubStack.clear();
			IfStack=0;//no number of if
			IsRunning=true;

			RunBlock(false);
		}
		catch(const string& error)
		{
			throw Error(error);
		}
	}

	void Script::RunBlock(bool Skip)
	{
#ifdef _HK_DEBUG_
		cout<<"\n\n------------------------------------------\nrunning\n------------------------------------------\n\n";
#endif
		bool IsBlockEnd=false;
		while(!Tokenizer.IsFinished() && IsRunning && RunLine(Skip));//runline returns false if end of block is reached
	}

	void Script::SkipLine()
	{
		//keeps getting next token until the line end is reached
		TokenId NextToken=Tokenizer.GetNextToken();
		while(NextToken != TOKEN_EOL && NextToken != TOKEN_EOF)
			NextToken=Tokenizer.GetNextToken();
		return;
	}
	//the next token must be EOL statement
	void Script::CheckEndOfLine(string after)
	{
		if(Tokenizer.GetNextToken() != TOKEN_EOL && (Tokenizer.GetNextToken() !=  TOKEN_EOF))
		{
			throw Error("Expected End of line after \'"+after+"\'");
		}
	}

	void Script::NumNumMatch(const Variable& v1,const Variable v2,const string& Operator)
	{
		//both v1 and v2 must be number
		if(v1.type != Variable::Type::VAR_NUM || v2.type != Variable::Type::VAR_NUM)
		{
			throw Error("Cannot apply operator "+Operator+" to string");
		}
	}
	void Script::StrStrMatch(const Variable& v1,const Variable v2,const string& Operator)
	{
		//both v1 and v2 must be number
		if(v1.type != Variable::Type::VAR_STR || v2.type != Variable::Type::VAR_STR)
		{
			throw Error("Cannot apply operator "+Operator+" to Numerical type");
		}
	}
	void Script::SameTypeMatch(const Variable& v1,const Variable v2,const string& Operator)
	{
		//both v1 and v2 must be number or both string
		if( 
			!( 
			(v1.type == Variable::Type::VAR_STR && v2.type == Variable::Type::VAR_STR) 
			||
			(v1.type == Variable::Type::VAR_NUM && v2.type == Variable::Type::VAR_NUM)
			))
		{
			throw Error("Cannot apply operator "+Operator+" to different Data type");
		}
	}
	bool Script::RunLine(bool Skip)//runs a line of code and returns
	{
#ifdef _HK_DEBUG_
		cout<<"runline with skip "<<Skip<<endl;
#endif
		//we first expect a line number. If the first thing in the line is a number, we ignore it
		Tokenizer.IgnoreToken(TOKEN_NUM);
		//now look as usual for statements and functions
		TokenId Token=Tokenizer.GetNextToken();
		switch(Token)
		{
		case TOKEN_STATEMENT:
			{
				switch(Tokenizer.GetLastStatementment())
				{
				case STMT_REM:
					{
						//skip all the things until we reach the end of the current line
						Tokenizer.SkipHardLine();//skip until we find \n or 0
						Tokenizer.GetNextToken();//this will skip the \n character also
						break;
					}
				case STMT_END:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						exit(0);
						break;
					}
				case STMT_WHILE:
					{
						int startpointer=Tokenizer.GetPointer();
						while(1)
						{
							//get the condition
							Variable Condition=Level1();
							CheckEndOfLine("condition");
							WhileStack++;
							bool condition=Skip || !Condition.GetIntVal();
							RunBlock(condition);//run the block inside if
							if(Tokenizer.GetLastToken() != TOKEN_STATEMENT || Tokenizer.GetLastStatementment() != STMT_REPEAT)
							{
								throw Error("Expected WEND after WHILE");
							}
							WhileStack--;//reduce the "while" stack
							CheckEndOfLine("WEND");
							if(condition)//if skip is true
								break;
							Tokenizer.SetPointer(startpointer);
						}
						break;
					}
				case STMT_FOR:
					{
						//if we are just skipping the block then get to the end of for, and skip the for block also
						if(Skip)
						{
							ForStack++;
							SkipLine();
							RunBlock(true);
							if(Tokenizer.GetLastToken() != TOKEN_STATEMENT || Tokenizer.GetLastStatementment() != STMT_NEXT)
							{
								throw Error("Expected Next after For");
							}
							ForStack--;
							CheckEndOfLine("Next");
							break;
						}
						//for a=1 to 12 step 2
						//the "for a=12 to " part is always constant... end and step value may change with each iteration

						//expect a variable
						TokenId NextToken;

						if(Tokenizer.GetNextToken() != TOKEN_VARIABLE)
						{
							throw Error("Expected variable name after for");
						}
						string curvarname=Tokenizer.GetLastVarName();
						//ForVarStack.push_back(curvarname);
						//expect a '='
						if(Tokenizer.GetNextToken() != TOKEN_OPERATOR || Tokenizer.GetLastOperator() != OPER_EQ)
						{
							throw Error("Expected =");
						}
						Variable val=Level1();
						if(val.type != Variable::Type::VAR_NUM)
						{
							throw Error("for start parameter can't be string");
						}
						varmanager.AddVariable(curvarname,val);//initialize it to this value
						//expect a "to"
						if(Tokenizer.GetNextToken() != TOKEN_STATEMENT || Tokenizer.GetLastStatementment() != STMT_TO)
						{
							throw Error("Expected 'TO'");
						}
						//initialize the array 
						Variable val2;
						int pointer=Tokenizer.GetPointer();//save the pointer
						while(1)
						{

							val2=Level1();
							if(val2.type != Variable::Type::VAR_NUM)
							{
								throw Error("for end parameter can't be string");
							}
							bool forward=(val2.GetNumVal()>val.GetNumVal());//are we iterating forward?
							double step=(forward)?1:-1;//-ve default step if it is going backwards
							if(Tokenizer.PeekNextToken() == TOKEN_STATEMENT && Tokenizer.GetLastStatementment() == STMT_STEP)
							{
								Tokenizer.GetNextToken();//consume the token
								Variable stp=Level1();
								if(val2.type != Variable::Type::VAR_NUM)
								{
									throw Error("for step parameter can't be string");
								}
								step=stp.GetNumVal();
							}
							//check if we should run the body
							bool shouldskip=false;
							if(forward && varmanager.GetVariable(curvarname).GetNumVal()>val2.GetNumVal())
								shouldskip=true;
							else if((!forward) && varmanager.GetVariable(curvarname).GetNumVal()<val2.GetNumVal())
								shouldskip=true;
							//cout<<"For variable name "<<curvarname<<" running from "<<val.GetNumVal()<<" to "<<val2.GetNumVal()<<" with step "<<step<<" with current value "<<varmanager.GetVariable(curvarname).GetNumVal()<<endl;
							RunBlock(shouldskip);
							//expect a next
							if(Tokenizer.GetLastToken() != TOKEN_STATEMENT || Tokenizer.GetLastStatementment() != STMT_NEXT)
							{
								throw Error("Expected Next after For");
							}
							CheckEndOfLine("Next");
							//now if the condition is met, then add value to the variable. Else, end the for loop
							if(shouldskip)
							{
								ForStack--;
								break;
							}

							varmanager.AddVariable(curvarname,Variable(varmanager.GetVariable(curvarname).GetNumVal()+step));

							Tokenizer.SetPointer(pointer);
						}
						break;


					}
				case STMT_IF:
					{
						bool ConditionFilled=false;//has the condition been fullfilled yet?
						//get the condition statement
						Variable Condition=Level1();
						//expect end of line after then
						CheckEndOfLine("IF Condition");
						IfStack++;
						//if we are already skipping then current true statement doesnot afffect it
						ConditionFilled=Condition.GetIntVal();
						HK_DEBUGP("skip is "<<Skip<<" and condition is "<<!Condition.GetIntVal()<<" and both is "<<(Skip || !ConditionFilled)<<endl);
						RunBlock(Skip || !ConditionFilled);//run the block inside if

						//if next token is elseif, get all the elseif statements
						while(Tokenizer.GetLastToken() == TOKEN_STATEMENT && Tokenizer.GetLastStatementment() == STMT_ELSEIF)
						{
							Variable Condition=Level1();
							CheckEndOfLine("Condition");
							//skip the condition has already been fullfilled.
							RunBlock(ConditionFilled || Skip || !Condition.GetIntVal());//run the block inside if condition has not fullfilled yet and skip is false
							ConditionFilled=ConditionFilled || Condition.GetIntVal();
						}
						if(Tokenizer.GetLastToken() == TOKEN_STATEMENT && Tokenizer.GetLastStatementment() == STMT_ELSE)
						{
							RunBlock(Skip || ConditionFilled);//skip is true if condition is already filled....else false
						}
						if(Tokenizer.GetLastToken() != TOKEN_STATEMENT || Tokenizer.GetLastStatementment() != STMT_ENDIF)
						{
							throw Error("Expected ENDIF after IF");
						}
						IfStack--;//reduce the "If" stack
						CheckEndOfLine("EndIf");
						break;
					}
				case STMT_JUMP:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						if(Tokenizer.GetNextToken() != TOKEN_NUM)
						{
							throw Error("Expected line number after gosub");
						}
						int lineno=static_cast<int>(Tokenizer.GetNumVal());
						if(LineNumbers.count(lineno) < 1)
						{
							throw Error("Line number "+NumToStr(lineno)+" not found");
						}
						//increase the token pointer until we encounter a line end
						TokenId NextToken=Tokenizer.GetNextToken();
						while(NextToken != TOKEN_EOL && NextToken != TOKEN_EOF)
							NextToken=Tokenizer.GetNextToken();
						//save the line number in the gosub stack
						GosubStack.push_back(Tokenizer.GetPointer());
						//jump to line number
						Tokenizer.SetPointer(LineNumbers[lineno]);

						return true;//directly return true...don't check EOL
					}
				case STMT_RETURN:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						if(GosubStack.size() == 0)
						{
							throw Error("Gosub stack empty!");
						}
						int Lineno=GosubStack[GosubStack.size()-1];
						GosubStack.pop_back();
						//jump to the previous position
						Tokenizer.SetPointer(Lineno);
						return true;
					}
				case STMT_ELSEIF:
				case STMT_ELSE:
				case STMT_ENDIF:
					{
						if(IfStack == 0)
							throw Error("Unexpected ELSEIF or ELSE or ENDIF");
						return false;//yes we have reached the end of a block
					}
				case STMT_NEXT:
					{
						if(ForStack == 0)
							throw Error("Unexpected NEXT");
						return false;
					}
				case STMT_REPEAT:
					{
						if(WhileStack==0)
							throw Error("Unexpected WEND");
						return false;
					}
				case STMT_GOTO:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						//get a line number
						if(Tokenizer.GetNextToken() != TOKEN_NUM)
						{
							throw Error("Expected line number after goto");
						}
						int lineno=static_cast<int>(Tokenizer.GetNumVal());
						if(LineNumbers.count(lineno) < 1)
						{
							throw Error("Line number "+NumToStr(lineno)+" not found");
						}
						//jump to line number
						Tokenizer.SetPointer(LineNumbers[lineno]);
						return true;
					}
				case STMT_INPUT:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						//SetColor(3);
						for(;;)
						{
							//int pp=Tokenizer.GetPointer();
							TokenId NextToken=Tokenizer.GetNextToken();
							if(NextToken != TOKEN_VARIABLE)
							{
								throw Error("Expected variable name in input");
							}
							string varname=Tokenizer.GetLastVarName();
							//get the input;
							Variable value;

							NextToken=Tokenizer.PeekNextToken();//see if the next token is % sign
							if(NextToken == TOKEN_OPERATOR && Tokenizer.GetLastOperator() == OPER_MOD)
							{
								Tokenizer.GetNextToken();//consume it
								double val;
								cin>>val;
								value.SetVal(val);
							}
							else
							{
								string val;
								getline(cin,val);
								value.SetVal(val);
							}
							varmanager.AddVariable(varname,value);
							//Variable[varname]=value;
							//now look for comma
							NextToken=Tokenizer.PeekNextToken();
							if(NextToken == TOKEN_EOL || NextToken== TOKEN_EOF)
								break;
							if(NextToken == TOKEN_OPERATOR && Tokenizer.GetLastOperator() == OPER_COMMA)
							{
								Tokenizer.GetNextToken();
								continue;
							}
							throw Error("Expected comma in input");
						}
						break;
					}
				case STMT_PRINT:
					{
						if(Skip)
						{
							SkipLine();//skip the line
							break;
						}
						//SetColor(3);
						//until we encounter eol or eof
						bool lastcomma=false;//was the las character a comma?
						for(;;)
						{
							TokenId NextToken=Tokenizer.PeekNextToken();
							if(NextToken == TOKEN_EOL || NextToken== TOKEN_EOF)//we allow only "print" to print a new line
								break;
							//expect the next thing to be an expression
							Variable val=Level1();
							if(val.type == Variable::Type::VAR_NUM)
								printf("%.15lg",val.GetNumVal());
							else
								printf("%s",val.GetStrVal().c_str());

							NextToken=Tokenizer.PeekNextToken();
							lastcomma=false;
							if(NextToken == TOKEN_EOL || NextToken== TOKEN_EOF)//we allow only "print" to print a new line
								break;
							if(NextToken == TOKEN_OPERATOR && Tokenizer.GetLastOperator() == OPER_COMMA)
							{
								printf(" ");//print  space if comma is specified
								lastcomma=true;
								Tokenizer.GetNextToken();
								continue;
							}
							throw Error("Expected comma in print");
						}
						if(!lastcomma)
							printf("\n");
						//SetColor(7);
						break;
					}
				} //switch laststatement
				break;
			}// case TOKEN_STATEMENT
		case TOKEN_VARIABLE:
			{
				if(Skip)
				{
					SkipLine();//skip the line
					break;
				}
				string Varname=Tokenizer.GetLastVarName();
#ifdef _HK_DEBUG_
				cout<<"will assign to variable "<<Varname<<endl;
#endif
				//expect an "=" sign
				if(Tokenizer.GetNextToken() != TOKEN_OPERATOR || Tokenizer.GetLastOperator() != OPER_EQ)
				{
					cout<<Tokenizer.GetLastToken()<<" and "<<Tokenizer.GetLastOperator()<<endl;
					throw Error("Expected '=' operators");
				}

				//get a value to set to the variable
				Variable value=Level1();
				varmanager.AddVariable(Varname,value);
				break;
			}
		case TOKEN_EOL:
		case TOKEN_EOF:
			return true;//don't do anything
		default:
			throw Error("Unexpected token at the very first");

		};
		if(Tokenizer.GetLastToken() == TOKEN_EOL)
			return true;
		else if(Tokenizer.GetLastToken() == TOKEN_EOF)
			return false;//end of block by EOF
		//cout<<Tokenizer.GetLastToken()<<" "<<Tokenizer.GetLastStatementment()<<endl;
		throw Error("Unexpected token at last");
	}

	/*
	this calculates the operators having lowest precedence like '+', '-', etc
	*/
	//currently, or operator has the lowest precedence
	Variable Script::Level1()
	{
		Variable var1=Level2();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)//if next token is not operator, return current value
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_OR:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level2();
					var1.SetVal(static_cast<double>(var1.GetIntVal() || var2.GetIntVal()));
					break;
				}
			default:
				//we reached here means although the next token is operator, it is not the operator we expected (may be comma) so, just return var1
				return var1;
			}
		}
	}
	//and operator has second lowest precedence
	Variable Script::Level2()
	{
		Variable var1=Level3();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)//if next token is not operator, return current value
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_AND:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level3();
					var1.SetVal(static_cast<double>(var1.GetIntVal() && var2.GetIntVal()));
					break;
				}
			default:
				//we reached here means although the next token is operator, it is not the operator we expected (may be comma) so, just return var1
				return var1;
			}
		}
	}
	//next come euqality and inequality operator
	//
	Variable Script::Level3()
	{
		Variable var1=Level4();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)//if next token is not operator, return current value
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_EQ:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level4();
					//cannot compare two different types
					SameTypeMatch(var1,var2,"=");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() == var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() == var2.GetStrVal()));
					break;
				}
			case OPER_NTEQ:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level4();
					//if they are not of same type, they are not equal
					//cannot compare two different types
					SameTypeMatch(var1,var2,"<>");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() != var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() != var2.GetStrVal()));
					break;
				}
			default:
				//we reached here means although the next token is operator, it is not the operator we expected (may be comma) so, just return var1
				return var1;
			}
		}
	}
	//and operator has second lowest precedence
	Variable Script::Level4()
	{
		Variable var1=Level5();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)//if next token is not operator, return current value
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_LT:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level5();
					//cannot compare two different types
					SameTypeMatch(var1,var2,"<");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() < var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() < var2.GetStrVal()));
					break;
					break;
				}
			case OPER_LTEQ:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level5();
					//if they are not of same type, they are not equal
					//cannot compare two different types
					SameTypeMatch(var1,var2,"<=");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() <= var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() <= var2.GetStrVal()));
					break;
				}
			case OPER_GT:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level5();
					//cannot compare two different types
					SameTypeMatch(var1,var2,">");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() > var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() > var2.GetStrVal()));
					break;
				}
			case OPER_GTEQ:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level5();
					//if they are not of same type, they are not equal
					//cannot compare two different types
					SameTypeMatch(var1,var2,">=");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(static_cast<double>(var1.GetNumVal() >= var2.GetNumVal()));
					else
						var1.SetVal(static_cast<double>(var1.GetStrVal() >= var2.GetStrVal()));
					break;
				}
			default:
				//we reached here means although the next token is operator, it is not the operator we expected (may be comma) so, just return var1
				return var1;
			}
		}
	}
	//addition, subtract operator have this level 4 precedence
	Variable Script::Level5()
	{
		Variable var1=Level6();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)//if next token is not operator, return current value
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_PLUS:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level6();
					SameTypeMatch(var1,var2,"+");
					if(var1.type == Variable::Type::VAR_NUM)
						var1.SetVal(var1.GetNumVal() + var2.GetNumVal());//add the numerical value and return it
					else
						var1.SetVal(var1.GetStrVal() + var2.GetStrVal());//else add the string value and return them
					break;
				}
			case OPER_MINUS:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level6();
					NumNumMatch(var1,var2,"-");//cannot subtract two strings
					var1.SetVal(var1.GetNumVal() - var2.GetNumVal());
					break;
				}
			default:
				//we reached here means although the next token is operator, it is not the operator we expected (may be comma) so, just return var1
				return var1;
			}
		}
	}
	//multiply, divide and modulo operator have this level 4 precedence
	Variable Script::Level6()

	{
		Variable var1=Level7();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_MULT:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level7();
					//if num2 cannot be a string. Every other condition is possible
					if(var2.type==Variable::Type::VAR_STR)
					{
						throw Error("Cannot apply operator * with second operand string");
					}
					if(var1.type == Variable::Type::VAR_NUM)
					{
						var1.SetVal(var1.GetNumVal() * var2.GetNumVal());//add the numerical value and return it
					}
					else
					{
						//else multiply the string by the vaue of var2
						int times=abs(static_cast<int>(var2.GetNumVal()));
						string newval="";
						for(int i=0;i<times;i++)
							newval+=var1.GetStrVal();
						var1.SetVal(newval);
					}
					break;
				}
			case OPER_INT_DIVIDE:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level7();
					NumNumMatch(var1,var2,"\\");//cannot divide two strings
					var1.SetVal(static_cast<int>(var1.GetNumVal() / var2.GetNumVal()));
					break;
				}
			case OPER_DIVIDE:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level7();
					NumNumMatch(var1,var2,"/");//cannot divide two strings
					var1.SetVal(var1.GetNumVal() / var2.GetNumVal());
					break;
				}
			case OPER_MOD:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Level7();
					NumNumMatch(var1,var2,"%");//cannot modulo two strings
					var1.SetVal(static_cast<int>(var1.GetNumVal()) % static_cast<int>(var2.GetNumVal()));
					break;
				}
			default:
				return var1;
			}
		}
	}
	//the exponential operator ^ has the highest precedence
	Variable Script::Level7()
	{
		Variable var1=Leveltop();
		while(true)
		{
			if(Tokenizer.PeekNextToken() != TOKEN_OPERATOR)
			{
				return var1;
			}
			switch(Tokenizer.GetLastOperator())
			{
			case OPER_POW:
				{
					Tokenizer.GetNextToken();//we accept the token (previously we just peeked)
					Variable var2=Leveltop();
					NumNumMatch(var1,var2,"^");//cannot raise the exponential of string
					var1.SetVal(pow(var1.GetNumVal(), var2.GetNumVal()));
					break;
				}
			default:
				return var1;
			}
		}
	}
	//this is the top level precedence like the variable name, or bracket, or value itself
	//unary operators like '-', 'not' also have highest precedence
	Variable Script::Leveltop()
	{
		TokenId Token=Tokenizer.GetNextToken();
		switch(Token)
		{
		case TOKEN_FUNCTION:
			{
				//we use function handler here. It throws string exceptions
				try
				{
					if(Tokenizer.GetNextToken() != TOKEN_OPERATOR || Tokenizer.GetLastOperator() != OPER_LPAREN)
					{
						throw Error("Expected ( after function name");
					}
					string FunctionName=Tokenizer.GetLastFunctionName();
					vector<const Variable> Params;
					TokenId NextToken=Tokenizer.PeekNextToken();

					//check if function with empty params
					if(NextToken != TOKEN_OPERATOR || Tokenizer.GetLastOperator() != OPER_RPAREN)
					{
						while(true)
						{
							Variable value=Level1();
							Params.push_back(value);

							NextToken=Tokenizer.PeekNextToken();

							if(NextToken == TOKEN_OPERATOR && Tokenizer.GetLastOperator() == OPER_RPAREN)
								break;
							else if(NextToken == TOKEN_OPERATOR && Tokenizer.GetLastOperator() == OPER_COMMA)
							{
								Tokenizer.GetNextToken();//consume the comma
							}
							else
								throw Error("Expected function closing bracket )");

						}
					}
					Tokenizer.GetNextToken();//consume the )
					//run the function
					return functionhandler.RunFunction(FunctionName,Params);
				}
				catch(const string& error)
				{
					throw Error(error);
				}

			}
		case TOKEN_VARIABLE:
			return varmanager.GetVariable(Tokenizer.GetLastVarName());
		case TOKEN_NUM:
			return Variable(Tokenizer.GetNumVal());//make a variable out of numerical value and return it
		case TOKEN_STR:
			return Variable(Tokenizer.GetStrVal());
		case TOKEN_OPERATOR:
			//it can br unary -, or (
			switch(Tokenizer.GetLastOperator())
			{
				//not  has lowesr priority.....
			case OPER_NOT:
				{
					//get the expression and minus it
					Variable Var=Level1();
					//if it is string, not its length. Else, convert the number to integer and not it
					if(Var.type==Variable::Type::VAR_NUM)
					{
						return Variable(static_cast<double>(!(static_cast<int>(Var.GetNumVal()))));
					}
					return Variable(static_cast<double>(!(Var.GetStrVal().size())));
				}
			case OPER_MINUS:
				{
					//get the expression and minus it
					Variable Var=Leveltop();
					if(Var.type != Variable::Type::VAR_NUM)
						throw Error("Cannot apply unary '-' to string");
					Var.SetVal(-1*Var.GetNumVal());
					return Var;
				}
			case OPER_LPAREN:
				{
					//get the statements inside the parenthesis
					Variable var=Level1();
					if(Tokenizer.GetNextToken() != TOKEN_OPERATOR || Tokenizer.GetLastOperator() != OPER_RPAREN)
					{
						throw Error("Unmatched parenthesis");
					}
					return var;
				}

			}
		default:
			throw Error("Token not recognized in top level");
		}
	}
	void Script::Runline(const string& Line)
	{
		Tokenizer.SetProgram(Line);
		Run();
	}

	void Script::Reset(bool clearvar,bool clearconst)
	{
		Tokenizer.SetProgram("");
		if(clearvar)
			varmanager.ClearAllVars();
		if(clearconst)
			varmanager.ClearAllConst();
	}
	Variable Script::GetValue(const string& varname)
	{
		return varmanager.GetVariable(varname);
	}
	double Script::GetNumValue(const string& varname)
	{
		return varmanager.GetVariable(varname).GetNumVal();
	}
	string Script::GetStrValue(const string& varname)
	{
		return varmanager.GetVariable(varname).GetStrVal();
	}
}