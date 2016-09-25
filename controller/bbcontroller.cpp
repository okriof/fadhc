#include "hwinterface.h"
#include <fstream>
#include <unistd.h>
#include "params.h"

// Simpler On-Off controller
int main()
{
	HwInterface hw;
	std::ofstream logstream("templog.log");
	logstream << "data=[\n";

	{
		const Params params;
		params.print(std::cout);
		std::cout << std::endl;
	}

	// TODO: move prediction filters to own class.
	// temperature prediction filters
	const unsigned int historyLength = 30;
	double* pcfilt = new double[historyLength];
	double* pd1filt = new double[historyLength];
	double* histTvals = new double[historyLength]; // index 0: oldest value

	// fill filters
	{
		std::ifstream filtv("pinvA");
		filtv.read((char*)pcfilt, sizeof(double)*historyLength);
		filtv.read((char*)pd1filt, sizeof(double)*historyLength);
		for (unsigned int ii = 0; ii < historyLength; ++ii)
			std::cout << ii << ".\t" << pcfilt[ii] << ",    \t" << pd1filt[ii] << std::endl;
	}


	HwInterface::SensorData sensdat = hw.readData();

	// fill temperature history buffer with current temperature value
	for (unsigned int ii = 0; ii < historyLength; ++ii)
		histTvals[ii] = sensdat.Tnedervaning;


	time_t nexttime = time(NULL);

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

		// filter (line adjustment)
		double pcval = 0;
		double pd1val = 0;
		for (unsigned int ii = 1; ii < historyLength; ++ii)
		{		// oldest value in histTval[0] removed, new value to last element
			histTvals[ii-1] = histTvals[ii];
			//std::cout << histTvals[ii-1] << ", "; //TODO remove debug
			pcval += histTvals[ii-1]*pcfilt[ii-1];
			pd1val += histTvals[ii-1]*pd1filt[ii-1];
		}
		histTvals[historyLength-1] = sensdat.Tnedervaning;
		pcval += histTvals[historyLength-1]*pcfilt[historyLength-1];
		pd1val += histTvals[historyLength-1]*pd1filt[historyLength-1];
		//std::cout << std::endl;  //TODO remove debug

		// predicted temperature 
		const double longPredT = pcval + pd1val*params.TctrlLookAhead; 
		const double shortPredT = pcval + pd1val*60.0*10.0; // 10 minutes prediction for burner start 


		// boiler hold
		if (boilerHold) // boiler not running
		{
			if (shortPredT < sensdat.Tmal-params.boilerHoldDiff/2) // temperature check for boiler start
				if (longPredT < sensdat.Tmal+params.boilerHoldDiff/2) // check: not immediate stop
					boilerHold = false;
		}
		else		// boiler running
		{
			if (longPredT > sensdat.Tmal+params.boilerHoldDiff/2) // temperature check for boiler stop
				if (shortPredT > sensdat.Tmal-params.boilerHoldDiff/2) // check: not immediate restart
					boilerHold = true;
		}
		hw.setBoilerHold(boilerHold);

		// log predicted temperature
		logdat.TframMal = ((boilerHold) ? shortPredT : longPredT); // save prediction for next switch

		// log values not used.
		logdat.uStatic = pcval;  //TODO remove debug
		logdat.uOutT = pd1val;
		logdat.uProp = histTvals[0];
		logdat.uInt = 0;
		logdat.shuntI = 0;


		// write log
		logdat.controlState = 1*boilerHold;
		logdat.write(logstream);		
	}


	delete[] pcfilt;
	delete[] pd1filt;
	delete[] histTvals;

	return 0;
}
