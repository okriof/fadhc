#include "params.h"
#include <fstream>

Params::Params()
  : t_samp(60)
{
	shuntSetInterval = readParam("params/shuntSetInterval", 1, 30);
	shuntIparam = readParam("params/shuntIparam", 0, 5.0/3600)*t_samp*shuntSetInterval;
	
	TctrlSetInterval = readParam("params/TctrlSetInterval", shuntSetInterval, 30);
	t_Tctrl = t_samp*TctrlSetInterval;

	TctrlPparam = readParam("params/TctrlPparam", 1, 100);
	TctrlIparam = readParam("params/TctrlIparam", 0, .01)*t_Tctrl;
	TctrlImaxT = readParam("params/TctrlImaxT", 0, 30);
	TctrlStaticGain = readParam("params/TctrlStaticGain", 0.2, 1.2);

	TctrlOutFwdA = readParam("params/TctrlOutFwdA", 0.0000001, 0.01);
	TctrlOutFwdB = readParam("params/TctrlOutFwdB", 0.0000001, 0.01);
	TctrlOutStatic = TctrlOutFwdB/TctrlOutFwdA;

	TctrlOutFwdA = 1-(TctrlOutFwdA*t_Tctrl);
	TctrlOutFwdB = TctrlOutFwdB*t_Tctrl;

	TctrlLookAhead = readParam("params/TctrlLookAhead", 1, 3*60*60);

	controlMode = readParam("params/controlMode", 0, 1);
	boilerHoldDiff = readParam("params/boilerHoldDiff", 0, 2);
}


void Params::print(std::ostream& os) const
{
	os << "t_samp: " << t_samp << "\n";
	os << "shuntSetInterval: " << shuntSetInterval << "\n";
	os << "shuntIparam: " << shuntIparam << " (file: " << shuntIparam/(t_samp*shuntSetInterval) << ")" << "\n";
	os << "TctrlSetInterval: " << TctrlSetInterval << "\n";
	os << "t_Tctrl: " << t_Tctrl << "\n";
	os << "TctrlPparam: " << TctrlPparam << "\n";
	os << "TctrlIparam: " << TctrlIparam << " (file: " << TctrlIparam/t_Tctrl << ")" << "\n";
	os << "TctrlImaxT: " << TctrlImaxT << "\n";
	os << "TctrlStaticGain: " << TctrlStaticGain << "\n";
	os << "TctrlOutFwdA: " << TctrlOutFwdA << " (file: " << (1-TctrlOutFwdA)/t_Tctrl << ")" << "\n";
	os << "TctrlOutFwdB: " << TctrlOutFwdB << " (file: " << TctrlOutFwdB/t_Tctrl << ")" << "\n";
	os << "TctrlOutStatic: " << TctrlOutStatic << "\n";
	os << "controlMode: " << controlMode << "\n";
	os << "boilerHoldDiff: +-" << boilerHoldDiff << "\n";
}

double Params::readParam(const char* paramFile, double lowerBound, double upperBound)
{
	std::ifstream ifs(paramFile);
	double var;
	ifs >> var;
	if (var < lowerBound) var = lowerBound;
	if (var > upperBound) var = upperBound;
	return var;
}
