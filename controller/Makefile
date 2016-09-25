all: bbcontroller controller weeksetting logTransmitter tempReceiver


controller : controller.cpp hwinterface.cpp hwinterface.h params.cpp params.h ../sensorfiles.h Makefile
	g++ -Wall -o controller hwinterface.cpp params.cpp controller.cpp 


bbcontroller : bbcontroller.cpp hwinterface.cpp hwinterface.h params.cpp params.h ../sensorfiles.h Makefile
	g++ -Wall -o bbcontroller hwinterface.cpp params.cpp bbcontroller.cpp 


weeksetting: weeksetting.cpp Makefile
	g++ -Wall -o weeksetting weeksetting.cpp


logTransmitter: logTransmitter.cpp Makefile
	g++ -Wall -o logTransmitter logTransmitter.cpp


tempReceiver: tempReceiver.cpp Makefile
	g++ -Wall -o tempReceiver tempReceiver.cpp


clean : 
	rm bbcontroller controller weeksetting logTransmitter tempReceiver


