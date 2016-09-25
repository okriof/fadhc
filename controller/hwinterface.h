#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include <iostream>



class TempSensorMedian
{
public:
	TempSensorMedian(const char* sensorpath);
	double read();

private:
	double val1, val2, val3;
	const char* sensorpath;
};




class HwInterface
{
public:
	HwInterface();

	struct SensorData
	{
		unsigned long timestamp;
		double Tpanna;
		double Tfram;
		double Tretur;
		double Tnedervaning;
		double TuteN;
		double TuteS;
		double shunt;
		double Tmal;
		unsigned long pelletsCnt;
		unsigned long elforbCnt;
	};

	struct LoggData
	{
		LoggData(const SensorData& s);
		unsigned long timestamp;
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
		unsigned int controlState;
		unsigned int controlMode;
		unsigned long pelletsCnt;
		unsigned long elforbCnt;

		void write(std::ostream& os);
	};

	HwInterface::SensorData readData();
	double setShunt(double frac);
	void setBoilerHold(bool hold);


private:
	double shuntPos;
	TempSensorMedian 
		STpanna,
		STfram,
		STretur,
		STnedervaning,
		STuteN,
		STuteS;
};



#endif
