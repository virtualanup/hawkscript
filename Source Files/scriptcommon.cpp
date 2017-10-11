#include"scriptcommon.hpp"

namespace hawk
{
	
	//common functions
	string NumToStr(int no)
	{
		char number[100];
		sprintf(number,"%d",no);
		return number;
	}


	string ToUpper(const string& Source)
	{
		string ret;
		for(int i=0;i<Source.size();i++)
			ret+=toupper(Source[i]);
		return ret;
	}
}