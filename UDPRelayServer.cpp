// UDPRelayServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <fstream>
#include <string>
using namespace std;
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")

//1 M = 1024 K = 1024*1024 byte
#define MAX_DATA_SIZE 50*1024

int _tmain(int argc, _TCHAR * argv[])
{
	printf(
		"UDP Broadcast Retransmission Server by liang, 20151125\n"
	);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET m_localSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int listenport, sendport;
	vector<sockaddr_in> addrs;
	ifstream ifs(".\\proxy.ini");
	string line;
	if (ifs)
	{
		ifs >> line >> line >> line;
		ifs >> listenport;
		printf("listenPort: %d\n", listenport);
		ifs >> sendport;
		printf("sendPort: %d\n", sendport);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sendport);

		printf("sendAddr:\n");
		while (!ifs.eof())
		{
			ifs >> line;
			printf("\t%s\n", line.c_str());
			addr.sin_addr.s_addr = inet_addr(line.c_str());
			addrs.push_back(addr);
		}
	}
	else
	{
		printf("error read proxy.ini, quit in 3 seconds.\n");
		Sleep(3000);
		return 0;
	}

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(listenport);
	localAddr.sin_addr.s_addr = ADDR_ANY;

	if (bind(m_localSocket, (sockaddr*)& localAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		closesocket(m_localSocket);
		return false;
	}

	char recvBuff[MAX_DATA_SIZE] = "";
	int recvNum = 0;

	//char ip[20] = "172.18.1.18";
	//int port = 3001;

	//sockaddr_in addr;
	//addr.sin_family = AF_INET;
	//addr.sin_port = htons(port);
	//addr.sin_addr.s_addr = inet_addr(ip);

	sockaddr remoteAddr;
	int len = sizeof(sockaddr);
	while (true)
	{
		ZeroMemory(recvBuff, MAX_DATA_SIZE);
		recvNum = recvfrom(m_localSocket, recvBuff, MAX_DATA_SIZE, 0, &remoteAddr, &len);
		for (size_t i = 0; i < addrs.size(); ++i)
		{
			sendto(m_localSocket, recvBuff, recvNum, 0, (sockaddr*)& addrs[i], sizeof(sockaddr));
		}
		printf("转发。\n");
	}

	return 0;
}

