#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <strsafe.h>
#include "Utils.h"
#include "Encryptions.h"

using namespace std;

class Elevate
{
public:

	bool RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey);

	bool RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey);

	static void DisableWow64Redirect(void);

	void GoHigherExecute();
};
