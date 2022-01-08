#include "HttpRequests.h"



HINTERNET HttpRequests::AddHeader(HINTERNET Wrequest, string header)
{
	if (!WinHttpAddRequestHeaders(Wrequest, (LPCWSTR)Utils::s2ws(header).c_str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	return Wrequest;
}

HINTERNET HttpRequests::AddDefaultHeaders(HINTERNET Wrequest, string clientId, bool ifFile)
{
	HINTERNET newSession;
	DWORD dwFlags =
		SECURITY_FLAG_IGNORE_UNKNOWN_CA |
		SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
	if (!WinHttpSetOption(Wrequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags)))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	if (ifFile)
	{
		if (!WinHttpAddRequestHeaders(Wrequest, L"Content-Type:multipart/form-data", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return NULL;
		}
	}
	else
	{
		if (!WinHttpAddRequestHeaders(Wrequest, L"Content-Type:application/json", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return NULL;
		}
	}

	if (!clientId.empty())
	{
		newSession = AddHeader(Wrequest, "Clientid: " + clientId);
		if (newSession == NULL)
			return newSession;
	}
	return Wrequest;
}

string HttpRequests::ReadDataFromResponse(HINTERNET Wrequest)
{
	DWORD dwStatusCode = 0, dwTemp;
	bool WresponseData, Wresponse;
	string fullData;
	DWORD sizeResp, downloaded = 0;
	LPSTR outContent;
	DWORD len_response = 0;
	Wresponse = WinHttpReceiveResponse(Wrequest, NULL);
	if (!Wresponse)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return "";
	}
	else
	{
		dwTemp = sizeof(dwStatusCode);
		if (!WinHttpQueryHeaders(Wrequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwStatusCode, &dwTemp, NULL))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		}
		if (dwStatusCode == 200)
		{
			do
			{
				sizeResp = 0;
				if (!WinHttpQueryDataAvailable(Wrequest, &sizeResp))
				{
#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
					return "";
				}
				outContent = (char*)malloc(sizeResp + 1);
				if (!outContent)
				{
#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
					return "";
				}
				else
				{
					ZeroMemory(outContent, sizeResp + 1);
					WresponseData = WinHttpReadData(Wrequest, (LPVOID)outContent, sizeResp, &downloaded);
					if (!WresponseData)
					{

#ifdef _DEBUG
						Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
						return "";
					}
					else
					{
						if (strlen(outContent) != 0)
						{
							fullData = fullData + outContent;
						}
					}
				}
			} while (sizeResp > 0);
		}
#if _DEBUG
		else
		{
			cout << "Status code that was received: " << dwStatusCode << endl;
		}
#endif		
	}
	return fullData;
}

HINTERNET HttpRequests::createFirstConnection(LPCWSTR fullDomain)
{
	HINTERNET Wopen, Wconnect;
	LPCWSTR useragent = L"Mozilla/5.0 (Windows Phone 10.0; Android 6.0.1; Microsoft; RM-1152) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Mobile Safari/537.36 Edge/15.15254";
	Wopen = WinHttpOpen(useragent, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (Wopen == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	INTERNET_PORT port = 3000;
	if (_wcsnicmp(fullDomain, (LPCWSTR)Utils::s2ws("127.0.0.1").c_str(), 25) != 0)
		Wconnect = WinHttpConnect(Wopen, fullDomain, INTERNET_DEFAULT_HTTPS_PORT, 0);
	else
		Wconnect = WinHttpConnect(Wopen, fullDomain, port, 0);
	if (Wconnect == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	return Wconnect;
}

string HttpRequests::SendGetRequest(string uriPath, HINTERNET clientRequestSession, string clientId)
{
	string fullData;
	HINTERNET Wrequest;
	Wrequest = WinHttpOpenRequest(clientRequestSession, L"GET", Utils::s2ws(uriPath).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (Wrequest == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return "";
	}

	Wrequest = AddDefaultHeaders(Wrequest, clientId);

	if (!WinHttpSendRequest(Wrequest, WINHTTP_NO_ADDITIONAL_HEADERS, -1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return "";
	}
	else
	{
		fullData = ReadDataFromResponse(Wrequest);
	}
	WinHttpCloseHandle(Wrequest);
	return fullData;
}

string HttpRequests::SendPostData(string data, HINTERNET clientRequestSession, string uriPath, string clientId)
{
	string fullData;
	HINTERNET Wrequest;

	Wrequest = WinHttpOpenRequest(clientRequestSession, L"POST", Utils::s2ws(uriPath).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (Wrequest == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	Wrequest = AddDefaultHeaders(Wrequest, clientId);

	if (!WinHttpSendRequest(Wrequest, WINHTTP_NO_ADDITIONAL_HEADERS, -1L, (LPVOID)data.c_str(), (DWORD)data.length(), (DWORD)data.length(), 0))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	else
		fullData = ReadDataFromResponse(Wrequest);
	WinHttpCloseHandle(Wrequest);
	return fullData;
}
