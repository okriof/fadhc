#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <iostream>
#include <arpa/inet.h>


struct LoggData // TODO: keep similar as in hwinterface.h
{
	LoggData(std::istream& iss);
	double timestamp;
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
	double controlState;
	double controlMode;
	double pelletsCnt;
	double elforbCnt;

	void write(std::ostream& os);
};

void LoggData::write(std::ostream& os)
{
	os << static_cast<unsigned long>(this->timestamp) << " ";
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

LoggData::LoggData(std::istream& iss)
{
	iss >> timestamp;
	iss >> Tpanna;
	iss >> Tfram;
	iss >> Tretur;
	iss >> Tnedervaning;
	iss >> TuteN;
	iss >> TuteS;
	iss >> shunt;
	iss >> Tmal;
	iss >> TframMal;
	iss >> shuntI;
	iss >> uStatic;
	iss >> uOutT;
	iss >> uProp;
	iss >> uInt;
	iss >> controlState;
	iss >> controlMode;
	iss >> pelletsCnt;
	iss >> elforbCnt;

	char semicolon;
	iss >> semicolon;
}


int main()
{
	const unsigned int broadcastPort = 9930;
	const char* broadcastAddr = "172.16.1.255";

	std::cout << "Pack Size: " << sizeof(LoggData) << std::endl;
	std::ifstream logstream("templog.log"); // data source
	if (!logstream.good())
		throw("Unable to open temperature log.");
	
	{ // read "data=["-line
		std::string str;
		logstream >> str;
		std::cout << str << std::endl;
	}


	// setup sockets for UDP broadcast
	int socketID;
	char socketData[16];
	sockaddr_in* si_other_p = (sockaddr_in*) socketData;
	const int socketDataLen = sizeof(*si_other_p);
	if ((socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		throw("socket");
	// set broadcast
	int broadcastPermission = 1; 	
	if (setsockopt(socketID, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
		throw("setsockopt() failed");
	si_other_p->sin_family = AF_INET;
	si_other_p->sin_port = htons(broadcastPort);
	if (inet_aton(broadcastAddr, &si_other_p->sin_addr)==0)
		throw("inet_aton() failed");




	for(;;)
	{
		std::streampos strpos = logstream.tellg();
		LoggData logpt(logstream);
		while (logstream.fail())
		{
			//std::cout << "EOF starting read at " << strpos << std::endl;
			std::cout << "." << std::flush;

			sleep(20); // wait and retry
			logstream.clear();
			logstream.seekg(strpos);
			logpt = LoggData(logstream);
		}
		
		logpt.write(std::cout); // display data

		// broadcast data
		if (sendto(socketID, (const char*) &logpt, sizeof(logpt), 0,
				(sockaddr*)socketData, socketDataLen)==-1)
			std::cerr << "Unable to send" << std::endl;
		
		sleep(1);
	}

}
