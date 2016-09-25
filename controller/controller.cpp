#include "hwinterface.h"
#include <fstream>
#include <unistd.h>
#include "params.h"

int main()
{
	HwInterface hw;
	std::ofstream logstream("templog.log");
	logstream << "data=[\n";

	double TctrlOutStaticCopy;

	{
		const Params params;
		TctrlOutStaticCopy = params.TctrlOutStatic;
		params.print(std::cout);
		std::cout << std::endl;
	}

	HwInterface::SensorData sensdat = hw.readData();

	time_t nexttime = time(NULL);

	// framledning
	int shuntIntervalCounter = 10000;
	double shuntSetting = 0;
	double shuntI = 0;

	double TfwdTarget = 30; // outgoing water temperature target value

	// temperatur
	int TctrlIntervalCounter = 10000;


	double TctrlI = 0;    // integrative 
	double TctrlOutFwdNorth = sensdat.TuteN * TctrlOutStaticCopy;
	double TctrlOutFwdSouth = sensdat.TuteS * TctrlOutStaticCopy;
	// outdoor temperature dependent (init to static value)

	double uStatic = 0;
	double uOutT = 0;
	double uProp = 0;
	double uInt = 0;

	bool boilerHold = false;
	


	for(;;)
	{
		// read parameters
		const Params params;

		// wait (constant sampling rate)
		while (time(NULL) < nexttime)
			sleep(3);
		nexttime += params.t_samp;
		

		// read sensors, initialize log data
		sensdat = hw.readData();
		HwInterface::LoggData logdat(sensdat);

		// boiler hold
		if (params.boilerHoldDiff > 0)
		{
			if (sensdat.Tnedervaning > sensdat.Tmal + params.boilerHoldDiff) 
				boilerHold = true;   // start hold when boilerHoldDiff above set value
			if (sensdat.Tnedervaning < sensdat.Tmal) 
				boilerHold = false;  // stop hold when below set value
			hw.setBoilerHold(boilerHold);
		}
		else  // disable boiler hold
		{
			boilerHold = false;
			hw.setBoilerHold(false);
		}

		// control mode
		logdat.controlMode = params.controlMode;
		switch(params.controlMode)
		{
		case 1:		// aim for boiler hold
			sensdat.Tmal += params.boilerHoldDiff; // aim warmer for boiler hold
			break;

		default:
			break;
		}




		// controller (indoor temp) sets desired supply temperature
		if (++TctrlIntervalCounter >= params.TctrlSetInterval)
		{
			TctrlIntervalCounter = 0;
			const double Terr = sensdat.Tmal - sensdat.Tnedervaning;

			// static gain
			uStatic = sensdat.Tmal/params.TctrlStaticGain;

			// outdoor temperature compensation
			TctrlOutFwdNorth = TctrlOutFwdNorth*params.TctrlOutFwdA +
				sensdat.TuteN*params.TctrlOutFwdB;
			TctrlOutFwdSouth = TctrlOutFwdSouth*params.TctrlOutFwdA +
				sensdat.TuteS*params.TctrlOutFwdB;
			uOutT = -(TctrlOutFwdNorth + TctrlOutFwdSouth);

			// proportional
			uProp = params.TctrlPparam * Terr;

			// integrational
			if (not(boilerHold))
				TctrlI += params.TctrlIparam * Terr; // avoid update if holding (winddown)
			if (TctrlI >  params.TctrlImaxT) TctrlI =  params.TctrlImaxT;
			if (TctrlI < -params.TctrlImaxT) TctrlI = -params.TctrlImaxT;
			uInt = TctrlI;
		}
		TfwdTarget = uStatic + uOutT + uProp + uInt;

		logdat.TframMal = TfwdTarget;
		logdat.uStatic = uStatic;
		logdat.uOutT = uOutT;
		logdat.uProp = uProp;
		logdat.uInt = uInt;
		


		// controller (framledningstemperatur)
		if (++shuntIntervalCounter >= params.shuntSetInterval)
		{
			shuntIntervalCounter = 0;
			shuntI += (TfwdTarget - sensdat.Tfram)*params.shuntIparam;
			
			if (shuntI < -0.5) shuntI = -0.5;
			if (shuntI >  0.5) shuntI =  0.5;  // TODO: parameter?

			if (sensdat.Tretur >= sensdat.Tpanna-2) 
				shuntSetting = 0.2; // avoid motion when boiler is off.
			else
			{
				shuntSetting = (TfwdTarget-sensdat.Tretur)/(sensdat.Tpanna-sensdat.Tretur);
				shuntSetting += shuntI;
			}
			
			
			std::cout << "Setting shunt " << shuntSetting << " corresp. "
				<< shuntSetting*6+1 << std::endl;                     // TODO: remove

			hw.setShunt(shuntSetting);
		}
		logdat.shuntI = shuntI;



		// write log
		logdat.controlState = 1*boilerHold;
		logdat.write(logstream);		
	}

	return 0;
}
