#ifndef LOGRECEIVER_H
#define LOGRECEIVER_H

#include <iostream>
#include <fstream>

class LogReceiver
{
public:
	LogReceiver();

	struct LoggData // TODO: keep similar as in hwinterface.h
	{
		double timestamp;
		double Tpanna;
		double Tfram;
		double Tretur;
		double Tnedervaning;
		double TuteN;
		double TuteS;
		double shunt;
		double Tmal;
		double TframMal;
		double shuntI;
		double uStatic;
		double uOutT;
		double uProp;
		double uInt;
		double controlState;
		double controlMode;
		double pelletsCnt;
		double elforbCnt;
		void write(std::ostream& os) const;
	};

	const LoggData& getDataWait();


private:
	LoggData lastData;
	// TODO: buffer
	const unsigned int packSize;

	std::ofstream logdatafile;

	int socketID;
	//char socketData[16];
	//int socketDataLen;
};

#endif
