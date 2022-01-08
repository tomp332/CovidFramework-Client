
#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <iostream>
#include "Utils.h"
#include "PublicStructures.h"
using namespace std;
#pragma comment(lib, "Winhttp")

class HttpRequests
{
public:

	HINTERNET AddHeader(HINTERNET Wrequest, string header);

	HINTERNET AddDefaultHeaders(HINTERNET Wrequest, string clientId = "", bool ifFile = false);

	string ReadDataFromResponse(HINTERNET Wrequest);

	HINTERNET  createFirstConnection(LPCWSTR fullDomain);

	string SendGetRequest(string uriPath, HINTERNET clientRequestSession, string clientId = "");
	
	string SendPostData(string data, HINTERNET clientRequestSession, string uriPath, string clientId = "");

};

