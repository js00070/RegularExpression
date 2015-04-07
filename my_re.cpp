#include "my_re.h"

void StatLink(Status* start, char key, Status* end)
{
	start->AddOut(key, end);
	//	end->AddIn(start,key);
}

void oprtCalc(stack<Expr>& exprST, stack<char>& oprtST)
{
	while (1)
	{
		if (oprtST.size() <= 1)
			break;
		Expr tmpExpr = exprST.top();
		char tmpch1 = oprtST.top();
		oprtST.pop();
		char tmpch2 = oprtST.top();
		switch (tmpch1)
		{
		case 'E':
			switch (tmpch2)
			{
			case 'E':
				exprST.pop();
				(exprST.top()).Link(tmpExpr);
				break;
			case '|':
				exprST.pop();
				(exprST.top()).Union(tmpExpr);
				break;
			case '[':
			case '(':
				oprtST.push(tmpch1);
				return;
				break;
			}
			break;
		case '*':
			(exprST.top()).Closure();
			break;
		case ')':
			oprtST.pop();
			oprtST.pop();
			oprtST.push('E');
			break;
		case '[':
			oprtST.pop();
			oprtST.pop();
			oprtST.push('E');
			(exprST.top()).Optional();
			break;
		}
	}
}

Edge::Edge(Status* s1, char key, Status* s2)
{
	MatchContent = key;
	this->Start = s1;
	this->End = s2;
}

//	void Status::AddIn(Status* S,char key)
//	{
//		InEdges.push_back(Edge(S,key,this));
//	}

void Status::AddOut(char key, Status* S)
{
	OutEdges.push_back(Edge(this, key, S));
}

void Status::e_LinkStat(Status* S)
{
	this->AddOut(epsilon, S);
	//		S->AddIn(this, epsilon);
}

Expr::Expr(Status* in1, Status* in2)
{
	this->Start = in1;
	this->End = in2;
}

void Expr::Link(Expr follow)
{
	(this->End)->e_LinkStat(follow.Start);
}

void Expr::Union(Expr follow)
{
	Status *newStart, *newEnd;
	newStart = new Status;
	newEnd = new Status;
	StatLink(newStart, epsilon, follow.Start);
	StatLink(newStart, epsilon, this->Start);
	StatLink(follow.End, epsilon, newEnd);
	StatLink(this->End, epsilon, newEnd);
	this->Start = newStart;
	this->End = newEnd;
}

void Expr::Closure()
{
	Status *newStat = new Status;
	StatLink(newStat, epsilon, Start);
	StatLink(End, epsilon, newStat);
	this->Start = newStat;
	this->End = newStat;
}

void Expr::Optional()
{
	this->Start->AddOut(epsilon,this->End);
}

NFA::NFA(char* InputStr)
{
	int i = 0;
	bool StrEnd = false;
	stack<Expr> exprST;
	stack<char> oprtST;//operator
	Expr tmpExpr;
	Status* tmpStat;
	char tmpch;
	while (!StrEnd)
	{
		switch (InputStr[i])
		{
		case '[':
		case '(':
			oprtST.push(InputStr[i]);
			i++;
			break;
		case ']':
		case ')':
			if (InputStr[i + 1] != '*')
			{
				oprtST.push(InputStr[i]);
				oprtCalc(exprST, oprtST);
			}
			else
			{
				oprtST.pop();
				oprtST.pop();
				oprtST.push('E');
				oprtST.push('*');
				oprtCalc(exprST, oprtST);
				i++;
			}
			i++;
			break;
		case '|':
			oprtST.push(InputStr[i]);
			i++;
			break;
		case epsilon:
			StrEnd = true;
			break;
		case '\\':
			i++;
		default://operator 'link'
			if (oprtST.empty() || oprtST.top() != 'E')
			{
				oprtST.push('E');
				tmpExpr.Start = new Status;
				tmpExpr.End = tmpExpr.Start;
				exprST.push(tmpExpr);
			}
			if (InputStr[i + 1] != '*')
			{
				tmpStat = new Status;
				tmpExpr = exprST.top();
				StatLink(tmpExpr.End, InputStr[i], tmpStat);
				tmpExpr.End = tmpStat;
				exprST.top() = tmpExpr;
			}
			else
			{
				oprtST.push('E');
				oprtST.push('*');
				tmpExpr.Start = new Status;
				tmpExpr.End = new Status;
				StatLink(tmpExpr.Start, InputStr[i], tmpExpr.End);
				exprST.push(tmpExpr);
				i++;
			}
			oprtCalc(exprST, oprtST);
			i++;
			break;
		}
	}
	this->Start = (exprST.top()).Start;
	(exprST.top()).End->FinalStatus = true;
	(this->Start)->BeginStatus = true;
}

NFA::~NFA()
{
	for(int i=0;i<this->ValidStats.size();i++)
		delete this->ValidStats[i];
}

void NFA::GetStatsList()
{
	hash_map<Status*, bool> vis1;
	hash_map<Status*, bool> vis2;
	hash_map<Status*, bool> ifValid;
	Status* tmpStat;
	queue<Status*> SQ;
	this->ValidStats.push_back(Start);
	SQ.push(this->Start);
	vis1[this->Start] = true;
	ifValid[this->Start] = true;
	while (!SQ.empty())
	{
		tmpStat = SQ.front();
		SQ.pop();
		list<Edge>::iterator it;
		for (it = tmpStat->OutEdges.begin(); it != tmpStat->OutEdges.end(); it++)
			if (!vis1[it->End])
			{
				SQ.push(it->End);
				vis1[it->End] = true;
				if (it->MatchContent != epsilon)
				{
					this->ValidStats.push_back(it->End);
					ifValid[it->End] = true;
				}
			}
	}
	SQ.push(this->Start);
	vis2[this->Start] = true;
	while (!SQ.empty())
	{
		tmpStat = SQ.front();
		SQ.pop();
		list<Edge>::iterator it;
		for (it = tmpStat->OutEdges.begin(); it != tmpStat->OutEdges.end(); it++)
			if (!vis2[it->End])
			{
				SQ.push(it->End);
				vis2[it->End] = true;
				if (it->MatchContent == epsilon && !ifValid[it->End])
					this->unValidStats.push_back(it->End);
			}
	}
}

vector<Edge> NFA::e_closure(Status* start)
{
	bool ifEnd = false;
	vector<Edge> res;
	hash_map<Status*, bool> vis;
	queue<Status*> SQ;
	Status* tmpStat;
	SQ.push(start);
	vis[start] = true;
	while (!SQ.empty())
	{
		tmpStat = SQ.front();
		SQ.pop();
		list<Edge>::iterator it;
		for (it = tmpStat->OutEdges.begin(); it != tmpStat->OutEdges.end(); it++)
		{
			if (!vis[it->End])
			{
				if (it->MatchContent == epsilon)
				{
					SQ.push(it->End);
					vis[it->End] = true;
					if (it->End->FinalStatus)
						ifEnd = true;
				}
				else
					if (tmpStat != start)
						res.push_back(*it);
			}
		}
	}
	if (ifEnd)
		start->FinalStatus = true;
	return res;
}

void NFA::DeleteEpsilon()
{
	this->GetStatsList();
	vector<Edge> OutEdgeList;
	for (int i = 0; i < this->ValidStats.size(); i++)
	{
		OutEdgeList = e_closure(ValidStats[i]);
		for (int j = 0; j < OutEdgeList.size(); j++)
			StatLink(ValidStats[i], OutEdgeList[j].MatchContent, OutEdgeList[j].End);
	}
	for (int i = 0; i < this->ValidStats.size(); i++)
		for (list<Edge>::iterator it = this->ValidStats[i]->OutEdges.begin(); it != this->ValidStats[i]->OutEdges.end();)
			if (it->MatchContent == epsilon)
				it = this->ValidStats[i]->OutEdges.erase(it);
			else
				it++;
	for (int i = 0; i < this->unValidStats.size(); i++)
		delete this->unValidStats[i];
}

DetStat::DetStat()
{
	
}

int DFA::BuildDict()
{
	int i,tmp,flag;
	list<Edge>::iterator it;
	for(i=0;i<LargestChar+1;i++)
		this->CharDict[i] = 0;
	for(i=0;i<this->nfa->ValidStats.size();i++)
		for(it=this->nfa->ValidStats[i]->OutEdges.begin();it!=this->nfa->ValidStats[i]->OutEdges.end();it++)
			this->CharDict[it->MatchContent] = 1;
	tmp = 1;
	flag = -1;
	for(i=0;i<LargestChar;i++)
	{
		if(this->CharDict[i]==1 && flag==-1)
		{
			flag*=-1;
			tmp++;
		}
		if(this->CharDict[i]==0 && flag==1)
			flag*=-1;
		if(flag==1)
		{
			this->CharDict[i] = tmp;
		}else
		{
			this->CharDict[i] = 0;
		}
	}
	return tmp;
}

DFA::DFA(char* InputStr)
{
	this->nfa = new NFA(InputStr);
	this->nfa->DeleteEpsilon();
	this->CompressedTableLength = this->BuildDict();
}
