#include "Encryptions.h"

string Encryptions::base64Encode(string plainText)
{
	return encrypt::Base64::Encode(plainText);
}

string Encryptions::base64Decode(string encoded)
{
	string decoded;
	encrypt::Base64::Decode(encoded, decoded);
	return decoded;
}

string Encryptions::hexToAscii(string hexString)
{
	string ascii = "";
	for (size_t i = 0; i < hexString.length(); i += 2) {
		string part = hexString.substr(i, 2);
		char ch = stoul(part, nullptr, 16);
		ascii += ch;
	}
	return ascii;
}
