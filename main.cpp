//main
#include "my_re.h"
#include <iostream>
using namespace std;

int main()
{
	DFA a("(fuck)|(fuckk)|(fuckkd)");
	int tmp = a.parser("fuckkdd");
	cout << tmp << endl;
	system("pause");
	return 0;
}
