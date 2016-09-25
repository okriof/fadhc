#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "../sensorfiles.h"

int setShunt(double frac);

int main()
{
	//srand(...)

	time_t nexttime = time(NULL);
	const time_t t_samp = 60;
	//std::ostream &outstr = std::cout;
	std::ofstream outstr("templog.log");
 
	double shuntSetting = 0;
	int shuntResetCount = 4;
	const int shuntResetInterval = 5;

	nexttime += t_samp;
	//for (unsigned int ii = 0; ii < 10; ii++)
	for (;;)
	{
		while (time(NULL) < nexttime)
			sleep(3);
		nexttime += t_samp;

		time_t t_curr = time(NULL);
		outstr << t_curr << " ";

		{
			std::ifstream tdata(SENS_T_PANNA);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
			std::ifstream tdata(SENS_T_FRAMLEDNING);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
			std::ifstream tdata(SENS_T_RETUR);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
			std::ifstream tdata(SENS_T_NEDERVANING);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
			std::ifstream tdata(SENS_T_UTE_N);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
			std::ifstream tdata(SENS_T_UTE_S);
			double tempnr;
			tdata >> tempnr;
			outstr << tempnr << " ";
		}

		{
		//	std::ifstream tdata("/mnt/1wire/FC.0000000000FB/910/duty1");
		//	double tempnr;
		//	tdata >> tempnr;
			outstr << shuntSetting << " ";
		}

		outstr << ";\n" << std::flush;

		if (++shuntResetCount >= shuntResetInterval)
		{
			shuntResetCount = 0;
			shuntSetting = ((double)rand())/RAND_MAX;
			std::cout << "Setting shunt " << shuntSetting << " corresp. " 
				<< shuntSetting*6+1 << std::endl;
			setShunt(shuntSetting);
		}

	}	

	return 0;
}


int setShunt(double frac)
{
	if (frac < 0) frac = 0;
	if (frac > 1) frac = 1;
	int servovalue = (790.0 + frac*(2160.0-790.0));

	{
		std::ofstream tpm("/mnt/1wire/FC.0000000000FB/910/tpm1c");
		tpm << "4";
	}
	{
		std::ofstream period("/mnt/1wire/FC.0000000000FB/910/period1");
		period << "20000";
	}
	{
		std::ofstream duty("/mnt/1wire/FC.0000000000FB/910/duty1");
		duty << servovalue;
		std::cout << "set servovalue " << servovalue << std::endl;
	}
	
	sleep(2);

	{
		std::ofstream duty("/mnt/1wire/FC.0000000000FB/910/duty1");
		duty << "0";
	}

	return servovalue;
}