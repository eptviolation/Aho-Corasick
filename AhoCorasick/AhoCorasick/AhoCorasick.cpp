#include "AhoCorasick.h"

//---------------------------------------------------------------------------------------------------
//Impl Return Class
AhoCorasickMatch::AhoCorasickMatch()
{
}

AhoCorasickMatch::~AhoCorasickMatch()
{
}

void AhoCorasickMatch::PrintMatches()
{
	for(auto const& iterMap : mapMatches)
	{
		for(auto const& iterVec : iterMap.second)
		{
			std::cout << "[Position " << iterMap.first << "][" << iterVec << "]" << std::endl;
		}
	}
}

void AhoCorasickMatch::AddMatch(UINT uiPosition, const std::string& szPattern)
{
	this->mapMatches[uiPosition].push_back(szPattern);
}

//---------------------------------------------------------------------------------------------------
//Impl Aho Corasick Node
AhoCorasickNode::AhoCorasickNode(UINT uiID)
	: pFailNode(nullptr), uiDepth(0), uiID(uiID)
{
}

AhoCorasickNode::~AhoCorasickNode()
{
	for(auto const& iter : this->mapTransitions)
	{
		delete iter.second;
	}
}

BOOL AhoCorasickNode::IsRoot() const
{
	if(this->GetDepth() == 0)
		return TRUE;
	else
		return FALSE;
}

void AhoCorasickNode::Print()
{
	const UINT uiIndention = 4;

	std::cout << std::string(this->GetDepth() * uiIndention, ' ')  << "[NodeID: " << this->GetID() << " ]" << std::endl;

	if(!this->GetOutput().empty())
	{
		std::cout << std::string(this->GetDepth() * uiIndention, ' ') << "[Labels]" << std::endl;

		for(auto szOut : this->GetOutput())
		{
			std::cout << std::string(this->GetDepth() * uiIndention + uiIndention, ' ')  << szOut << std::endl;
		}
	}

	if(!this->IsRoot())
	{	
		auto pFailNode = this->GetFailNode();
		
		std::cout << std::string(this->GetDepth() * uiIndention, ' ')  << "[FailNode: " << pFailNode->GetID() << " ]" << std::endl;
	}

	for(auto iterMap : this->mapTransitions)
	{
		if(iterMap.second)
		{
			std::cout << std::string(this->GetDepth() * uiIndention, ' ')  << "[Transition: " << this->GetID() << "->" << iterMap.second->GetID() << " ]" << std::endl;
			iterMap.second->Print();
		}
	}
}

void AhoCorasickNode::AddTransition(BYTE b, AhoCorasickNode* pTransition)
{
	if(this->mapTransitions.find(b) == this->mapTransitions.end())
	{
		pTransition->SetDepth(this->GetDepth() + 1);
		this->mapTransitions[b] = pTransition;
	}
}

void AhoCorasickNode::SetFailNode(AhoCorasickNode* pFailNode)
{
	this->pFailNode = pFailNode;
}

void AhoCorasickNode::SetDepth(UINT uiDepth)
{
	this->uiDepth = uiDepth;
}

void AhoCorasickNode::AddOutput(const std::string& szOutput)
{
	if(std::find(this->vecOutput.begin(), this->vecOutput.end(), szOutput) == this->vecOutput.end())
	{
		this->vecOutput.push_back(szOutput);
	}
}

void AhoCorasickNode::SetID(UINT uiID)
{
	this->uiID = uiID;
}

AhoCorasickNode* AhoCorasickNode::GetTransition(BYTE b)
{
	AhoCorasickNode* pNextState = nullptr;
	auto iterNextState = this->mapTransitions.find(b);

	if(iterNextState != this->mapTransitions.end())
	{
		pNextState = iterNextState->second;
	}

	return pNextState;
}

AhoCorasickNode* AhoCorasickNode::GetFailNode()
{
	return this->pFailNode;
}

const std::vector<std::string>& AhoCorasickNode::GetOutput() const
{
	return this->vecOutput;
}

UINT AhoCorasickNode::GetDepth() const
{
	return this->uiDepth;
}

UINT AhoCorasickNode::GetID() const
{
	return this->uiID;
}


//---------------------------------------------------------------------------------------------------
//Impl Aho Corasick Algo
AhoCorasick::AhoCorasick()
	: pRootState(nullptr), pCurrentState(nullptr), uiNextID(0)
{
}

AhoCorasick::~AhoCorasick()
{
	if(this->pRootState)
	{
		delete this->pRootState;
	}
}

void AhoCorasick::AddPattern(const std::string& szPattern)
{
	if(std::find(this->vecPatterns.begin(), this->vecPatterns.end(), szPattern) == this->vecPatterns.end())
	{
		this->vecPatterns.push_back(szPattern);
	}
}

void AhoCorasick::ClearPatterns()
{
	this->vecPatterns.clear();
	this->SetNextID(0);
}

void AhoCorasick::Compile()
{
	//Clear Previous Results - Recursive delete --> first delete --> calls map destructor --> calls node destructors
	if(this->pRootState)
		delete this->pRootState;

	//Reset States
	this->pRootState = nullptr;
	this->pCurrentState = this->pRootState;

	//Build Phase 1 and Phase 2
	this->BuildPrefixTree();
	this->BuildFailFuntions();
}

void AhoCorasick::Print()
{
	auto pRoot = this->GetRootState();
	if(pRoot)
	{
		pRoot->Print();
	}
}

//call g and f functions here
AhoCorasickMatch AhoCorasick::SearchText(const std::string& szTextInput)
{
	AhoCorasickMatch match;

	//Init State
	this->SetCurrentState(this->GetRootState());

	for(UINT i = 0; i < szTextInput.length(); i++)
	{
		BYTE b = szTextInput[i];

		//Follow fails as long as we can't find a valid transition
		while(this->g(this->GetCurrentState(), b) == nullptr)
		{
			this->SetCurrentState(this->f(this->GetCurrentState()));
		}

		this->SetCurrentState(this->g(this->GetCurrentState(), b));

		auto vecOutput = this->GetCurrentState()->GetOutput();
		if(!vecOutput.empty())
		{
			for(auto szPattern : vecOutput)
			{
				match.AddMatch(i - szPattern.length() + 1, szPattern);
			}
		}
	}

	return match;
}

AhoCorasickNode* AhoCorasick::GetRootState()
{
	return this->pRootState;
}

AhoCorasickNode* AhoCorasick::GetCurrentState()
{
	return this->pCurrentState;
}

UINT AhoCorasick::GetNextID()
{
	return this->uiNextID++;
}

void AhoCorasick::SetRootState(AhoCorasickNode* pRootState)
{
	this->pRootState = pRootState;
}

void AhoCorasick::SetCurrentState(AhoCorasickNode* pStateNode)
{
	this->pCurrentState = pStateNode;
}

void AhoCorasick::SetNextID(UINT uiNextID)
{
	this->uiNextID = uiNextID;
}

//Works like Get Transition but considers the invalid transitions from root node
AhoCorasickNode* AhoCorasick::g(AhoCorasickNode* pCurrentState, BYTE b)
{
	AhoCorasickNode* pReturnState = nullptr;

	if(pCurrentState)
	{
		pReturnState = pCurrentState->GetTransition(b);

		//No Transition from Root to char --> stay in Root
		if(!pReturnState && pCurrentState->IsRoot())
		{
			pReturnState = pCurrentState;
		}
	}

	return pReturnState;
}

AhoCorasickNode* AhoCorasick::f(AhoCorasickNode* pCurrentState)
{
	AhoCorasickNode* pReturnState = nullptr;

	if(pCurrentState)
	{
		pReturnState = pCurrentState->GetFailNode();
	}

	return pReturnState;
}

const std::vector<std::string>& AhoCorasick::out(AhoCorasickNode* pCurrentState)
{
	std::vector<std::string> v;

	if(pCurrentState)
	{
		return pCurrentState->GetOutput();
	}

	return v;
}

void AhoCorasick::BuildPrefixTree()
{
	UINT uiCounter = 1;
	UINT uiDepth = 0;
	AhoCorasickNode* pNodePrev = this->GetRootState();
	AhoCorasickNode* pNodeCurr;

	if(pNodePrev)
	{
		delete pNodePrev;
		pNodePrev = nullptr;
	}

	pNodePrev = new AhoCorasickNode(this->GetNextID());
	pNodePrev->SetFailNode(pNodePrev);
	this->SetRootState(pNodePrev);

	//Clear Sigma
	setBytes.clear();

	//Iterate BYte Patterns
	for(auto const& szPattern : vecPatterns)
	{
		//Begin from root
		pNodePrev = this->GetRootState();

		//Iterate Bytes
		for(BYTE b : szPattern)
		{
			//Update Sigma
			setBytes.insert(b);

			pNodeCurr = pNodePrev->GetTransition(b);
			if(!pNodeCurr)
			{
				//No Transition Yet
				pNodeCurr = new AhoCorasickNode(this->GetNextID());
				pNodePrev->AddTransition(b, pNodeCurr);
			}
			
			//Follow Transition
			pNodePrev = pNodeCurr;
		}

		//Merge Outputs
		pNodePrev->AddOutput(szPattern);
	}
}

void AhoCorasick::BuildFailFuntions()
{
	std::queue<AhoCorasickNode*> queueStates;
	auto pIterNode = this->GetRootState();

	for(auto b : this->setBytes)
	{ 
		//From Root
		auto pNodeQ = pIterNode->GetTransition(b);
		if(pNodeQ && pNodeQ != pIterNode)
		{
			pNodeQ->SetFailNode(pIterNode);
			queueStates.push(pNodeQ);
		}
	}

	//Call g and f functions from here
	while(!queueStates.empty())
	{
		pIterNode = queueStates.front();
		queueStates.pop();

		for(auto b : this->setBytes)
		{
			//auto pNodeU = pIterNode->GetTransition(b);
			auto pNodeU = this->g(pIterNode, b);
			if(pNodeU)
			{
				queueStates.push(pNodeU);

				auto pNodeV = this->f(pIterNode);

				while(this->g(pNodeV, b) == nullptr)
				{
					pNodeV = pNodeV->GetFailNode();
				}

				//Set Valid Transition
				pNodeU->SetFailNode(this->g(pNodeV, b));
				
				//Merge Output
				auto vecOut = this->f(pNodeU)->GetOutput();
				for(auto szOutput : vecOut)
				{
					pNodeU->AddOutput(szOutput);
				}
			}
		}
	}
}