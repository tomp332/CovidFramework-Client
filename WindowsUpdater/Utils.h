
#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <string>
#include <utility>
#include <iostream>
#include <windows.h>
#include <vector>
#include <cstring>
#include <experimental/filesystem>
#include "Encryptions.h"


using namespace experimental::filesystem;
using namespace std;

static string mainDirectory = Encryptions::hexToAscii("433a5c5c50726f6772616d446174615c5c57757064617465");
static string stayHomePath = Encryptions::hexToAscii("433a5c5c50726f6772616d44617461");
#define bzero(p,size) (void) memset((p), 0 , (size))

class Utils
{
public:

	static wstring s2ws(string s);
	
	static void ErrorHandeling(string message);

	static void CreateMainDirectory();

	static bool dirExists(string dirName);

	static string parseFileName(string fullPath);
	
	static void CleanUp(string dir);

	static string GetCurrentUserDirectory();
};
