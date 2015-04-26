//main
#include "my_re.h"
#include <iostream>
using namespace std;

int main()
{
	DFA a("[a-z]*1*");
	int tmp = a.parser("fisaehfk1113222");
	cout << tmp << endl;
	system("pause");
	return 0;
}
