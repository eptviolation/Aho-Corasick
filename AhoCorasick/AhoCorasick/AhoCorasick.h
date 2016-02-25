#ifndef __AHOCORASICK_H__
#define __AHOCORASICK_H__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>

//Windows typdef
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//---------------------------------------------------------------------------------------------------
//Dec Return Class
class AhoCorasickMatch
{
protected:
	std::map<UINT, std::vector<std::string> > mapMatches;

public:
	AhoCorasickMatch();
	virtual ~AhoCorasickMatch();

	//Methods
	void PrintMatches();
	void AddMatch(UINT uiPosition, const std::string& szPattern);
};

//---------------------------------------------------------------------------------------------------
//Dec Aho Corasick Node
class AhoCorasickNode
{
protected:
	UINT								uiID;						
	UINT								uiDepth;
	std::vector<std::string>			vecOutput;
	std::map<BYTE, AhoCorasickNode*>	mapTransitions;
	AhoCorasickNode*					pFailNode;

public:
	AhoCorasickNode(UINT uiID);
	virtual ~AhoCorasickNode();

	//Methods
	BOOL IsRoot() const;
	void Print();
	
	//Setter
	void AddTransition(BYTE b, AhoCorasickNode* pTransition);
	void SetFailNode(AhoCorasickNode* pFailNode);
	void SetDepth(UINT uiDepth);	
	void AddOutput(const std::string& szOutput);
	void SetID(UINT uiID);

	//Getter
	AhoCorasickNode* GetTransition(BYTE b);
	AhoCorasickNode* GetFailNode();
	UINT GetDepth() const;
	const std::vector<std::string>& GetOutput() const;
	UINT GetID() const;

};

//---------------------------------------------------------------------------------------------------
//Dec Aho Corasick Algo
class AhoCorasick
{
protected:
	UINT						uiNextID;
	std::vector<std::string>	vecPatterns;
	std::set<BYTE>				setBytes;		//SIGMA

	AhoCorasickNode* pRootState;
	AhoCorasickNode* pCurrentState;

public:
	AhoCorasick();
	virtual ~AhoCorasick();

	//Preparation
	void AddPattern(const std::string& szPattern);
	void ClearPatterns();
	void Compile();
	void Print();

	AhoCorasickMatch SearchText(const std::string& szTextInput);

protected:
	//Getter
	AhoCorasickNode* GetRootState();
	AhoCorasickNode* GetCurrentState();
	UINT GetNextID();

	//Setter
	void SetRootState(AhoCorasickNode* pRootState);
	void SetCurrentState(AhoCorasickNode* pStateNode);
	void SetNextID(UINT uiNextID);

	//Internal State Transitions Functions
	AhoCorasickNode* g(AhoCorasickNode* pCurrentState, BYTE b);
	AhoCorasickNode* f(AhoCorasickNode* pCurrentState);

	const std::vector<std::string>& out(AhoCorasickNode* pState);

	//Construction
	void BuildPrefixTree();		//Phase 1
	void BuildFailFuntions();	//Phase 2
};

#endif