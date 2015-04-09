//my regular expression

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <queue>
#include <vector>
#include <stack>
#include <hash_map>
#include <utility>
#include <set>

using namespace std;

#define LargestChar 255

const char epsilon = '\0';

struct Edge;
struct Status;
void StatLink();
void oprtCalc();

struct Edge
{
	char MatchContent;
	Status* Start;
	Status* End;

	Edge(Status* s1, char key, Status* s2);
};

struct Status
{
//	list<Edge> InEdges;
	list<Edge> OutEdges;
	bool BeginStatus = false;
	bool FinalStatus = false;

//	void AddIn(Status* S,char key);
	void AddOut(char key, Status* S);
	void e_LinkStat(Status* S);
};

struct Expr
{
	Status* Start;
	Status* End;

	Expr(Status* in1 = NULL, Status* in2 = NULL);
	void Link(Expr follow);
	void Union(Expr follow);
	void Closure();
	void Optional();
};

struct NFA
{
	Status* Start;
	vector<Status*> ValidStats;
	vector<Status*> unValidStats;

	NFA(char* InputStr);
	~NFA();
	void GetStatsList();
	vector<Edge> e_closure(Status* start);
	void DeleteEpsilon();
};

struct DetStat
{
	bool BeginStatus;
	bool FinalStatus;
	int CompressedTableLength;
	vector<DetStat*>* CompressedTable;
	DetStat(int length);
	void SetAsBegin();
	void SetAsFinal();
};

struct DFA
{
	NFA* nfa;
	vector<DetStat> StatusList;
	int CharDict[LargestChar+2];
	int CompressedTableLength;
	
	int BuildCharDict();
	DetStat* AddStatus();

	DFA(char* InputStr);
};
