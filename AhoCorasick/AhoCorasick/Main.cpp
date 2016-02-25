#include <string>
#include <fstream>
#include <sstream>
#include "AhoCorasick.h"

void Test()
{
	//-----------------------------------------------------
	//Default Test
	AhoCorasick aho;

	aho.AddPattern("he");
	aho.AddPattern("she");
	aho.AddPattern("his");
	aho.AddPattern("hers");

	aho.Compile();
	aho.Print();

	AhoCorasickMatch match = aho.SearchText("ushers");
	match = aho.SearchText("ushers");
	match.PrintMatches();

	//-----------------------------------------------------
	//Homework
	aho.ClearPatterns();

	aho.AddPattern("ccddc");
	aho.AddPattern("cdcdc");
	aho.AddPattern("cdc");
	aho.AddPattern("cd");

	aho.Compile();
	aho.Print();

	match = aho.SearchText("ccdcddcdcdddcdcdcdccdcd");
	match.PrintMatches();
}

void Help(const std::string& szProgramName)
{
	std::cout << "usage: " << szProgramName << " -p <patternfile.txt> -t <text>" << std::endl;
}

int main(int argc, char* argv[])
{
	if(argc == 2)
	{
		std::string szArg1 = argv[1];
		if(szArg1 == "test")
		{
			Test();
			return 0;
		}
	}

	if(argc != 5)
	{
		Help(argv[0]);
		return 0;
	}

	//Arguments
	std::string szArg1 = argv[1];
	std::string szArg2 = argv[2];
	std::string szArg3 = argv[3];
	std::string szArg4 = argv[4];

	//Streams
	std::ifstream fileInputStream;
	std::string szPatternLine;

	AhoCorasick ahoAutomat;
	AhoCorasickMatch ahoMatch;

	if(szArg1 != "-p" && szArg3 != "-t") 
	{
		Help(argv[0]);
		return 0;
	}

	fileInputStream.open(szArg2.c_str(), std::ios::in);
	if(fileInputStream.fail())
	{
		std::cout << "Failed at opening file [" << szArg2 << "]" << std::endl;
		return -1;
	}
	
	//Load Patterns
	while(std::getline(fileInputStream, szPatternLine))
	{
		if(szPatternLine.length() == 0)
			continue;

		std::cout << "[Loading Patterns][" << szPatternLine << "]" << std::endl;
		ahoAutomat.AddPattern(szPatternLine);
	}

	//Compile and Print
	std::cout << "[Compiling Aho-Corasick State Machine]" << std::endl << std::endl;
	ahoAutomat.Compile();
	ahoAutomat.Print();

	//Find Matches and Print
	std::cout << "[Matching Results]" << std::endl << std::endl;
	ahoMatch = ahoAutomat.SearchText(szArg4);
	ahoMatch.PrintMatches();

	fileInputStream.close();

	return 0;
}