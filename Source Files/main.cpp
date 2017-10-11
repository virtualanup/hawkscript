/*
the hawk engine scripting language version 1.0
http://hawkscript.sourceforge.net/
this is free(as in free beer) code. You can use it in any way you like.
this is a part of hawk engine
(http://hawkengine.sourceforge.net/ )
*/
#define _HAWK_DEBUG_
#include<iostream>
#include<string>
#include<vector>
#include<conio.h>
#include<map>
#include<fstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include<Windows.h>
#include"script.hpp"
#include"tokenizer.hpp"

using namespace std;

string trim(string s)
{ 
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}
int main()
{
	hawk::Script Script;
	string line;
	cout<<"The hawk Script\nCopyleft 2013 Anup pokhrel and Manish Sharma\nno rights reserved\n\n"<<endl;
	for(;;)
	{
		try
		{
			cout<<">>> ";
			getline(cin,line);
			if(line.size() >= 4 && hawk::ToUpper(line.substr(0,4)) == "LOAD")
			{
				string filename=trim(line.substr(4,line.size()-4));
				string program,line;
				ifstream in(filename.c_str());
				if(!in.good())
				{
					throw string("Error loading input file "+filename);
				}
				while(getline(in,line))
					program+=line+"\n";
				Script.SetProgram(program);
				Script.Run();
				Script.Reset();
			}
			else if(hawk::ToUpper(line) == "CLEAR")
			{
				cout<<"Script States Cleared"<<endl;
				Script.Reset();
			}
			else if(hawk::ToUpper(line) == "EXIT")
				exit(0);
			else
				Script.Runline(line);
		}
		catch(const hawk::Error& Err)
		{
			if(Err.LineNo != -1)
				cout<<"Unrecoverable Error : "<<Err.ErrorText<<" on line "<<Err.LineNo<<endl;
			else
				cout<<"Unrecoverable Error : "<<Err.ErrorText<<endl;
		}
		catch(const string& Err)
		{
			cout<<"Unrecoverable Error : "<<Err<<endl;
		}
		cout<<endl;
	}
	cout<<"Press any key to continue . . .";
	getch();
	return 0;
}