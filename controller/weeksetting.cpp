#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>


void tToDH(time_t timeo, double& hod, unsigned int& weekday)
{
	const time_t daymodulo = 60*60*24;
	const time_t weekmodulo = 7*daymodulo;	

	hod = ((double)(timeo % daymodulo))/3600.0;
	weekday = (timeo % weekmodulo)/daymodulo+1;
}



double targetT(time_t timeo)
{
	double hod;
	unsigned int weekday;
	tToDH(timeo, hod, weekday);

	const double awayT = 17; 
	const double stdT = 20;
	const double nightT = 18.5;
	double morningT = 20.5;

	double setT = stdT;
	//std::cout << "W: " << weekday << ", H: " << hod << std::endl;

	// nattsänkning
	if (hod > 23 || hod <= 6.5)
		setT = nightT;
	// morning ramp
	if (hod > 5.5 && hod <= 6.5)
		setT = nightT + (morningT-nightT)*(hod-5.5)/1.0;
	// morning
	if (hod > 6.5 && hod <= 7.5)
		setT = morningT;
	// long morning
	if (weekday >= 6 && hod > 6.5 && hod <= 8.5)
		setT = morningT;



	// dagsänkning
	if (weekday <= 5)
	{
		if (hod > 7.5 && hod <= 18.5)
			setT = awayT;
		// afternoon ramp
		if (hod > 17.0 && hod <= 19)
			setT = awayT + (stdT-awayT)*(hod-17.0)/2.0;
	}

	return setT;
}



int main()
{
	// plot
	if (false)
	{
		std::ofstream of("plotdata.m");
		of << "data = [";
		for (unsigned int tc = 0; tc < 7*24*60*60; tc += 60)
			of << tc/3600.0 << " " << targetT(tc) << ";\n";
		of << "];\n";
		of << "h = axes;\n";
		of << "plot(data(:,1),data(:,2)); grid on;\n";
		of << "set(h,'XTickMode','manual');\n";
		of << "set(h,'XTick',0:24:7*24);\n";
	}
	const time_t timeoffset = 0*60*60 + 3*24*60*60 + 1.0*60*60; 
		// 1.0 h delay-compensation

	for (;;)
	{
		time_t t_curr = time(NULL) + timeoffset;
		double setT = targetT(t_curr);
		double hod;
		unsigned int weekday;
		tToDH(t_curr, hod, weekday);
		std::cout << "D: " << weekday << ",  tod: " << (int)hod << ":" << (int)((hod-(int)hod)*60.0) << ",  setT: " << setT << std::endl;

		{
			std::ofstream tsetStream("targetTemp");
			tsetStream << setT << std::flush;
		}

		sleep(30);
	}
	return 0;
}
