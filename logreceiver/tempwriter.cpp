#include "logreceiver.h"
#include <iostream>

int main()
{
	LogReceiver logRec;

	std::cout << "Receiving data:" << std::endl;
	for(;;)
		logRec.getDataWait().write(std::cout);

	return 0;
}

