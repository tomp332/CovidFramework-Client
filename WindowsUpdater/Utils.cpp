#include "Utils.h"
#include <ShlObj.h>

namespace fs = experimental::filesystem;

wstring Utils::s2ws(string s)
{
	wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}

void Utils::ErrorHandeling(string message)
{
	#ifdef _DEBUG
		cout << "[-] Error " << message << ", Err.number: " << GetLastError() << endl;
	#endif
}

void Utils::CreateMainDirectory()
{
	if (!CreateDirectoryA(mainDirectory.c_str(), NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
		#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
			mainDirectory = stayHomePath;
		}
	}
}

bool Utils::dirExists(string dirName)
{
	DWORD ftyp = GetFileAttributesA(dirName.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}

string Utils::parseFileName(string fullPath)
{
	const size_t last_slash_idx = fullPath.find_last_of("\\/");
	if (string::npos != last_slash_idx)
	{
		fullPath.erase(0, last_slash_idx + 1);
	}
	return fullPath;
}

void Utils::CleanUp(string dir)
{
	path pathToDelete(dir);
	remove_all(pathToDelete);
}

string Utils::GetCurrentUserDirectory()
{
	WCHAR path[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) != 0)
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	wstring Wpath = wstring(path);
	string fullPath = string(Wpath.begin(), Wpath.end());
	return fullPath;
}
