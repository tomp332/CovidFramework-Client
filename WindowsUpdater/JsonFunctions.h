
#pragma once
#include <string>
#include <iostream>
#include "PublicStructures.h"

using namespace std;

class JsonFunctions
{
public:

	static string CreateJsonLoginData(PublicStructers::client clientInfo);

	static string CreatePostJson(string fullData, bool jsonObject=false, bool encryptionNeeded=true);
};

