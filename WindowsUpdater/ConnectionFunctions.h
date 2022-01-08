#pragma once
#include <netlistmgr.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include "PublicStructures.h"
#include "HttpRequests.h"
#include "Utils.h"
#include "JsonFunctions.h"
using namespace std;

#pragma comment(lib,"wininet.lib")

class ConnectionFunctions
{
private:
	HttpRequests WinHttpRequests;
	enum class INTERNET_STATUS
	{
		CONNECTED,
		DISCONNECTED,
		CONNECTED_TO_LOCAL,
		CONNECTION_ERROR
	};
	INTERNET_STATUS IsConnectedToInternet();

public:

	bool GetConnectionState();

	PublicStructers::ClientSession ToolAuth(PublicStructers::client clientInfo, HINTERNET clientRequestSession);

};

