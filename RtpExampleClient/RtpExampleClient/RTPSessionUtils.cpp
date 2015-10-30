#include "RTPSessionUtils.h"

namespace jrtplib
{
	RTPSessionUtils::RTPSessionUtils()
	{
#ifdef WIN32
		WSADATA dat;
		WSAStartup(MAKEWORD(2, 2), &dat);
#endif
	}

	RTPSessionUtils::~RTPSessionUtils()
	{
#ifdef WIN32
		WSACleanup();
#endif
	}

	int RTPSessionUtils::CreateDefault(uint16_t port)
	{
		RTPUDPv4TransmissionParams transparams;
		RTPSessionParams sessparams;
		sessparams.SetOwnTimestampUnit(1.0 / 10.0); // 时间戳单位，必须设置
		transparams.SetPortbase(port); // port 必须是偶数
		return base_type::Create(sessparams, &transparams);

		base_type::SetDefaultPayloadType(0);
		base_type::SetDefaultTimestampIncrement(0);
		base_type::SetDefaultMark(false);
	}

	int RTPSessionUtils::AddDestination(const std::string &ip, uint16_t port)
	{
		return base_type::AddDestination(RTPIPv4Address(ntohl(inet_addr(ip.c_str())), port));
	}

	int RTPSessionUtils::DeleteDestination(const std::string &ip, uint16_t port)
	{
		return base_type::DeleteDestination(RTPIPv4Address(ntohl(inet_addr(ip.c_str())), port));
	}

	int RTPSessionUtils::GetAddrFromSource(RTPSourceData *dat, uint32_t& ip, uint16_t& port)
	{
		if (dat->IsOwnSSRC())
			return -1;

		if (dat->GetRTPDataAddress() != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if (dat->GetRTCPDataAddress != 0)
		{
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort() - 1;
		}
		return 0;
	}

	void RTPSessionUtils::OnNewSource(RTPSourceData *dat)
	{
		uint32_t ip;
		uint16_t port;

		if (GetAddrFromSource(dat, ip, port))
			return;

		RTPIPv4Address dest(ip ,port);
		base_type::AddDestination(dest);

		std::cout << "OnNewSource Adding destination " << IPtoString(ip) << ";" << port << std::endl;
	}

	void RTPSessionUtils::
}