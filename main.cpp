//main
#include "my_re.h"
#include <iostream>
using namespace std;

int main()
{
	NFA a = NFA("(fuck)*|(shit)*");
	a.DeleteEpsilon();
	system("pause");
	return 0;
}
