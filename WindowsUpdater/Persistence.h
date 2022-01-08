#pragma once
#include <windows.h>
#include <string>
#include <taskschd.h>
#include <comutil.h>
#include <comdef.h>
#include "Utils.h"
#include "HttpRequests.h"
#include "UploadDownload.h"

#pragma comment(lib, "taskschd.lib")

using namespace std;

class Persistence
{

private:
	UploadDownload DownloadUpload;
	PublicStructers::client clientInfo;

public:

	Persistence(PublicStructers::client clientInformation);

	bool CreateTask(string path);
};

