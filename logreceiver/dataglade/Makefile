dataglade : dataglade.o logreceiver.o timesel.o setTempTransmitter.o Makefile
	g++ -Wall -o dataglade dataglade.o logreceiver.o timesel.o setTempTransmitter.o `pkg-config gtkmm-3.0 --libs`

logreceiver.o : ../logreceiver.cpp ../logreceiver.h Makefile
	g++ -Wall -c -o logreceiver.o ../logreceiver.cpp -I ../

setTempTransmitter.o : ../setTempTransmitter.cpp ../setTempTransmitter.h Makefile
	g++ -Wall -c -o setTempTransmitter.o ../setTempTransmitter.cpp -I ../

dataglade.o : dataglade.cpp ../logreceiver.h timeSel.h Makefile
	g++ -Wall -c -o dataglade.o dataglade.cpp `pkg-config gtkmm-3.0 --cflags`

timesel.o : timeSel.cpp timeSel.h Makefile
	g++ -Wall -c -o timesel.o timeSel.cpp `pkg-config gtkmm-3.0 --cflags`


clean : 
	rm -f *.o dataglade


