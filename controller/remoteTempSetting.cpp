#include <iostream>
#include <fstream>
#include <unistd.h>
#include "../sensorfiles.h"

int main()
{
	const double highTemp = 19.0;
	const double lowTemp = 10.0;

	int reading = 0;
	int oldReading = 1; // Make sure desired temp is set on startup
	int equalReadings = 0;

	// Setup PIO in correct mode.
	{
		std::ofstream pioc(CTRL_REMOTE_SETUP);
		pioc << "18"; // Input pio, internal pull up.
	}

	std::cout << "Started. Looking for external temperature commands..." << std::endl;
	
	for(;;)
	{
		// read new setting
		{
			std::ifstream pio(SENS_REMOTE_TEMP);
			pio >> reading;
		}

		if (reading == oldReading)
		{
			equalReadings = equalReadings + 1;
		}
		else
		{
			oldReading = reading;
			equalReadings = 0;
		}

		if (equalReadings == 5)
		{ // A few equal readings in a row, a new temperature is desired
			if (oldReading == 1)
			{ // Passive high
				std::cout << "Setting low temperature." << std::endl;
				std::ofstream confF("targetTemp");
				confF << lowTemp;
			}
			else
			{ // Active low
				std::cout << "Setting high temperature." << std::endl;
				std::ofstream confF("targetTemp");
				confF << highTemp;
			}
		}
	
		// Keep counter limited
		if (equalReadings > 100)
		{
			equalReadings = 100;
		}

		sleep(10);
	}

	return 0;
}


