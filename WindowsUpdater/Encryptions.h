#pragma once
#include "Base64.h"
#include <string>
#include <iostream>
using namespace std;
class Encryptions
{
public:
	static string base64Encode(string plainText);
	static string base64Decode(string encoded);
	static string hexToAscii(string hexString);
};
