#include "hwinterface.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>

#include "../sensorfiles.h"



TempSensorMedian::TempSensorMedian(const char* sensorpath)
  : val1(0), val2(0), val3(0), sensorpath(sensorpath)
{
		std::ifstream tdata(sensorpath);
		double tempnr;
		tdata >> tempnr;
		val1 = tempnr;
		val2 = tempnr;
		val3 = tempnr;
}


double TempSensorMedian::read()
{
	val3 = val2;
	val2 = val1;

	std::ifstream tdata(sensorpath);
	tdata >> val1;

	double v1 = val1;
	double v2 = val2;
	double v3 = val3;

	if (v2 < v1) std::swap(v1, v2);
	if (v3 < v2) std::swap(v2, v3);
	if (v2 < v1) std::swap(v1, v2);

	return v2;
}



HwInterface::HwInterface()
: shuntPos(0),
  STpanna(SENS_T_PANNA),
  STfram(SENS_T_FRAMLEDNING),
  STretur(SENS_T_RETUR),
  STnedervaning(SENS_T_NEDERVANING),
  STuteN(SENS_T_UTE_N),
  STuteS(SENS_T_UTE_S)
{
	this->setShunt(0);
}


double HwInterface::setShunt(double frac)
{
	const double shuntMaxStep = 0.15;
	// range check
	if (frac > shuntPos + shuntMaxStep) frac = shuntPos + shuntMaxStep;
	if (frac < shuntPos - shuntMaxStep) frac = shuntPos - shuntMaxStep;
	if (frac < 0) frac = 0;
	if (frac > 1) frac = 1;
	int servovalue = (790.0 + frac*(2160.0-790.0));

	{
		std::ofstream tpm(CTRL_SHUNT_TPM);
		tpm << "4";
	}
	{
		std::ofstream period(CTRL_SHUNT_PERIOD);
		period << "20000";
	}
	{
		std::ofstream duty(CTRL_SHUNT_DUTY);
		duty << servovalue;
		std::cout << "set servovalue " << servovalue << std::endl; // TODO: remove debug
	}
	
	sleep(1);

	{
		std::ofstream duty(CTRL_SHUNT_DUTY);
		duty << "0";
	}

	shuntPos = frac;
	return frac;
}


void HwInterface::setBoilerHold(bool hold)
{
	std::ofstream boilHold(CTRL_BOILER_HOLD);

	if (hold)
		boilHold << "1";
	else
		boilHold << "0";
}



HwInterface::SensorData HwInterface::readData()
{
	SensorData sensdata;
	time_t t_curr = time(NULL);
	sensdata.timestamp = t_curr;

	sensdata.Tpanna = STpanna.read();
	sensdata.Tfram = STfram.read();
	sensdata.Tretur = STretur.read();
	sensdata.Tnedervaning = STnedervaning.read();
	sensdata.TuteN = STuteN.read();
	sensdata.TuteS = STuteS.read();


	sensdata.shunt = shuntPos;
		
	// read temp
	{
		double targetTemp;
		std::ifstream tttemp("targetTemp"); // TODO: filename
		tttemp >> targetTemp;
		if (targetTemp < 15) targetTemp = 15;
		if (targetTemp > 24) targetTemp = 24;
		sensdata.Tmal = targetTemp;
	}


	{
		std::ifstream tdata(SENS_PELLETS_CNT);
		unsigned long tempnr;
		tdata >> tempnr;
		sensdata.pelletsCnt = tempnr;
	}

	{
		std::ifstream tdata(SENS_ELFORB_CNT);
		unsigned long tempnr;
		tdata >> tempnr;
		sensdata.elforbCnt = tempnr;
	}
	
	return sensdata;
}

HwInterface::LoggData::LoggData(const SensorData& s)
: timestamp(s.timestamp), Tpanna(s.Tpanna), Tfram(s.Tfram), Tretur(s.Tretur),
  Tnedervaning(s.Tnedervaning), TuteN(s.TuteN), TuteS(s.TuteS), shunt(s.shunt),
  Tmal(s.Tmal), 
  TframMal(0), shuntI(0), uStatic(0), uOutT(0), uProp(0), uInt(0),
  controlState(0), controlMode(0),
  pelletsCnt(s.pelletsCnt), elforbCnt(s.elforbCnt)
{ }

void HwInterface::LoggData::write(std::ostream& os)
{
	os << this->timestamp << " ";
	os << this->Tpanna << " ";
	os << this->Tfram << " ";
	os << this->Tretur << " ";
	os << this->Tnedervaning << " ";
	os << this->TuteN << " ";
	os << this->TuteS << " ";
	os << this->shunt << " ";
	os << this->Tmal << " ";
	os << this->TframMal << " ";
	os << this->shuntI << " ";
	os << this->uStatic << " ";
	os << this->uOutT << " ";
	os << this->uProp << " ";
	os << this->uInt << " ";
	os << this->controlState << " ";
	os << this->controlMode << " ";
	os << this->pelletsCnt << " ";
	os << this->elforbCnt << " ";
	os << ";\n" << std::flush;
}

