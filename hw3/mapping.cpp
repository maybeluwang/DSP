#include <iostream>
using namespace std;
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>

#ifndef _map_big5
#define _map_big5
class map_big5
{
public:
	vector<string> phonetic[37];
	vector<string> all;
	map_big5();
	~map_big5();
	void input(const char*);
	void mapping(string, string);
	void output(const char*);
};
#endif


int main ()
{
	map_big5 ZhuYin;
	ZhuYin.input("Big5-ZhuYin.map");
	ZhuYin.output("ZhuYin-Big5.map");
	return 0;

}


map_big5::map_big5()
{
	
}
map_big5::~map_big5()
{
	for (int i=0; i<37; i++)
		phonetic[i].clear();
}
void map_big5::input(const char* filename)
{
	ifstream fin;
	fin.open(filename, ios::in|ios::binary);
	string character;
	string line;
	while(fin>>character>>line)
	{
		mapping(character, line);
	}
	fin.close();
}


void map_big5::mapping(string character, string ZhuYin)
{
	all.push_back(character);
	if((ZhuYin[0]==-93)&&(ZhuYin[1]>-96)&&(ZhuYin[1]<-69))
	{
		phonetic[ZhuYin[1]+106].push_back(character);
	}
	else if ((ZhuYin[0]==-93)&&(ZhuYin[1]>115)&&(ZhuYin[1]<127))
	{
		phonetic[ZhuYin[1]-116].push_back(character);
	}
	
	for (int i=0; i<ZhuYin.length(); i++)
	{
		if (ZhuYin[i]==47)
		{
			if((ZhuYin[i+1]==-93)&&(ZhuYin[i+2]>-96)&&(ZhuYin[i+2]<-69))
			{
				if (ZhuYin[i+2]!=ZhuYin[1])
					phonetic[ZhuYin[1]+106].push_back(character);					
			}
			else if ((ZhuYin[i+1]==-93)&&(ZhuYin[i+2]>115)&&(ZhuYin[i+2]<127))
			{
				if (ZhuYin[i+2]!=ZhuYin[1])
					phonetic[ZhuYin[1]-116].push_back(character);
				
				
			}
		}
	}
	
}
void map_big5::output(const char* filename)
{
	ofstream fout;
	fout.open(filename, ios::out|ios::binary);
	if (fout.is_open())
	{
		for (int i=0; i<11; i++)
		{
			if (phonetic[i].size()!=0)
			{
				fout<<(char)0xa3<<(char)(116+i)<<' '<<' ';
				for (int j=0; j<phonetic[i].size(); j++)
					fout<<phonetic[i][j]<<' ';
				fout<<endl;
			}
						
		}
		for (int i=11; i<37; i++)
		{
			
			if (phonetic[i].size()!=0)
			{
				fout<<(char)0xa3<<(char)(150+i)<<' '<<' ';
				for (int j=0; j<phonetic[i].size(); j++)
					fout<<phonetic[i][j]<<' ';
				fout<<endl;
			}
			
		}
		for (int i=0; i<all.size(); i++)
		{
			fout<<all[i]<<' '<<' '<<all[i]<<endl;
		}
	}
	fout.close();
}
