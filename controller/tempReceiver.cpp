#include <iostream>
#include <fstream>

#include <arpa/inet.h>
#include <unistd.h>


int main()
{
	std::cout << "Started. Receiving temperature settings..." << std::endl;

	int socketID;

	struct sockaddr_in si_me;
	struct sockaddr_in si_other;
	unsigned int slen=sizeof(si_other);

	if ((socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		throw("socket");

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(9931);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socketID, (sockaddr*)&si_me, sizeof(si_me))==-1)
		throw("bind");

	double recvTemp = 19.0;

	for (;;)
	{
		if (recvfrom(socketID, (char*)(&recvTemp), sizeof(recvTemp), 0, (sockaddr*)(&si_other), &slen)==-1)
			throw("recvfrom()");
		std::cout << "Received packet from " 
		   << inet_ntoa(si_other.sin_addr) << ":" 
		   << ntohs(si_other.sin_port) 
		<< " T: " << recvTemp << std::endl;

		if (recvTemp < 5) recvTemp = 5;
		if (recvTemp > 25) recvTemp = 25;

		{
			std::ofstream confF("targetTemp");
			confF << recvTemp;
		}
	}

	close(socketID);

	return 0;
}


