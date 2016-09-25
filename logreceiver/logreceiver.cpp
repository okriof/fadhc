#include "logreceiver.h"

#include <iostream>

#include <arpa/inet.h>
#include <unistd.h>

LogReceiver::LogReceiver()
 :  packSize(sizeof(LoggData)), logdatafile("binlogfile", std::ofstream::app | std::ofstream::ate)
{
	std::cout << "Size: " << packSize << std::endl;

	struct sockaddr_in si_me;
	
	if ((socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		throw("socket");

	//memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(9930);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socketID, (sockaddr*)&si_me, sizeof(si_me))==-1)
		throw("bind");

/*	for (i=0; i<10; i++)
	{
		if (recvfrom(s, buf, BUFLEN, 0, (sockaddr*)(&si_other), &slen)==-1)
			throw("recvfrom()");
		std::cout << "Received packet from " 
		   << inet_ntoa(si_other.sin_addr) << ":" 
		   << ntohs(si_other.sin_port) << " data: " << buf << std::endl;
	}

	close(s);*/
}



const LogReceiver::LoggData& LogReceiver::getDataWait()
{
	struct sockaddr_in si_other;
	unsigned int slen=sizeof(si_other);

	// TODO: buffering
	if (recvfrom(socketID, (char*)(&lastData), packSize, 0, (sockaddr*)(&si_other), &slen)==-1)
			throw("recvfrom()");

	logdatafile.write((char*)(&lastData), packSize);
	logdatafile.flush();

	return lastData;
}



void LogReceiver::LoggData::write(std::ostream& os) const
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
	os << this->pelletsCnt/7996.0 << " ";
	os << this->elforbCnt/1000.0 << " ";
	os << ";\n" << std::flush;
}


