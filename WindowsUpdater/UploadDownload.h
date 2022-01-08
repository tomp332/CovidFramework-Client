#pragma once
#include "PublicStructures.h"
#include "HttpRequests.h"

class UploadDownload
{
public:

	HttpRequests WinHttpRequests;

	bool upload_to_server(string filePath, PublicStructers::client clientInfo);

	bool upload_to_client(string filename, PublicStructers::client  clientInfo, bool fromDownloadsPath = true, string specialSaveDirectory = "");

	void uploadToServerThread(string filePath, PublicStructers::client clientInfo);
	
	void downloadToClientThread(string filename, PublicStructers::client clientInfo);
};
