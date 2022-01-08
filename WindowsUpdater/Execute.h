#pragma once
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <cstring>
#include <wincred.h>
#include "iostream"
#include "string"
#include "Utils.h"
#include "JsonFunctions.h"
#include "HttpRequests.h"
#pragma comment(lib, "Credui.lib")
using namespace std;


class Execute
{
public:
	static void CreateProcessExeucte(string file, string params = "");
	
	static void PromptUser(PublicStructers::client clientInfo);
};

