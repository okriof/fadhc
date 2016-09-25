#ifndef SET_TEMP_TRANSMITTER_H
#define SET_TEMP_TRANSMITTER_H



class SetTempTransmitter
{
public:
	SetTempTransmitter();

	void setTemp(double targetTemp);




private:
	int socketID;
	char socketData[16];
	int socketDataLen;
	double lastSentTemp;
};




#endif
