#ifndef PARAMS_H
#define PARAMS_H
#include <iostream>

class Params 
{
public:
	Params();
	void print(std::ostream& os) const;

	const time_t t_samp;

	int shuntSetInterval;
	double shuntIparam;

	int TctrlSetInterval;
	double t_Tctrl; // derived

	double TctrlPparam;
	double TctrlIparam;
	double TctrlImaxT;
	double TctrlStaticGain;

	double TctrlOutFwdA;
	double TctrlOutFwdB;

	double TctrlOutStatic;  // derived

	double TctrlLookAhead; // look ahead time (seconds)

	int controlMode; // 0: standard, 1: aim for boilerHold
	double boilerHoldDiff; // set to 0 to disable



private:
	double readParam(const char* paramFile, double lowerBound, double upperBound);
};








#endif
