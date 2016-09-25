#include "setTempTransmitter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <iostream>
#include <arpa/inet.h>



SetTempTransmitter::SetTempTransmitter()
{
	lastSentTemp = 19.0;
	const unsigned int port = 9931;
	//const char* broadcastAddr = "172.16.1.8";
	const char* broadcastAddr = "172.16.1.12";

	// setup sockets for UDP	
	sockaddr_in* si_other_p = (sockaddr_in*) socketData;
	socketDataLen = sizeof(*si_other_p);
	if ((socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		throw("socket");
	// set broadcast
	// int broadcastPermission = 1; 	
	// if (setsockopt(socketID, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
	//	throw("setsockopt() failed");
	si_other_p->sin_family = AF_INET;
	si_other_p->sin_port = htons(port);
	if (inet_aton(broadcastAddr, &si_other_p->sin_addr)==0)
		throw("inet_aton() failed");

	
}




void SetTempTransmitter::setTemp(double targetTemp)
{
	if (targetTemp == lastSentTemp) return;

	if (sendto(socketID, (const char*) &targetTemp, sizeof(targetTemp), 0,
			(sockaddr*)socketData, socketDataLen)==-1)
		std::cerr << "Unable to send" << std::endl;

	lastSentTemp = targetTemp;
}



