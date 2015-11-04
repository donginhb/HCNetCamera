// client
#include <iostream>
#include "RTPSessionUtils.h"
using namespace jrtplib;

void main()
{
	int status;
	RTPSessionUtils sess;
	status = sess.CreateDefault(8000);
	if(status)
	{
		std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
		return;
	}
	status = sess.AddDestination("192.168.3.50", 8888);
	status = sess.AddDestination("192.168.3.51", 6666);
	if(status)
	{
		std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
		return;
	}

	while (1)
	{
		std::string buf;
		std::cout << "Input send data:" ;
		std::cin >> buf;

		sess.SendPacket((void*)buf.c_str(), buf.length(), 0, false, 0);
		if(status)
		{
			std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
			continue;
		}
	}

	system("pause");
}
