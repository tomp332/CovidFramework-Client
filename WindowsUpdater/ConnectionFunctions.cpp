#include "ConnectionFunctions.h"

ConnectionFunctions::INTERNET_STATUS ConnectionFunctions::IsConnectedToInternet()
{
	INTERNET_STATUS connectedStatus = INTERNET_STATUS::CONNECTION_ERROR;
	HRESULT hr = S_FALSE;
	try
	{
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			INetworkListManager* pNetworkListManager;
			hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, __uuidof(INetworkListManager), (LPVOID*)&pNetworkListManager);
			if (SUCCEEDED(hr))
			{
				NLM_CONNECTIVITY nlmConnectivity = NLM_CONNECTIVITY::NLM_CONNECTIVITY_DISCONNECTED;
				VARIANT_BOOL isConnected = VARIANT_FALSE;
				hr = pNetworkListManager->get_IsConnectedToInternet(&isConnected);
				if (SUCCEEDED(hr))
				{
					if (isConnected == VARIANT_TRUE)
						connectedStatus = INTERNET_STATUS::CONNECTED;
					else
						connectedStatus = INTERNET_STATUS::DISCONNECTED;
				}
				if (isConnected == VARIANT_FALSE && SUCCEEDED(pNetworkListManager->GetConnectivity(&nlmConnectivity)))
				{
					if (nlmConnectivity & (NLM_CONNECTIVITY_IPV4_LOCALNETWORK | NLM_CONNECTIVITY_IPV4_SUBNET | NLM_CONNECTIVITY_IPV6_LOCALNETWORK | NLM_CONNECTIVITY_IPV6_SUBNET))
					{
						connectedStatus = INTERNET_STATUS::CONNECTED_TO_LOCAL;
					}
				}
				pNetworkListManager->Release();
			}
		}
		CoUninitialize();
	}
	catch (...)
	{
		connectedStatus = INTERNET_STATUS::CONNECTION_ERROR;
	}
	return connectedStatus;
}

bool ConnectionFunctions::GetConnectionState()
{
	INTERNET_STATUS connectedStatus = INTERNET_STATUS::CONNECTION_ERROR;
	connectedStatus = IsConnectedToInternet();
	switch (connectedStatus)
	{
	case INTERNET_STATUS::CONNECTED:
		return true;
		break;
	case INTERNET_STATUS::DISCONNECTED:
		return false;
		break;
	default:
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
}

PublicStructers::ClientSession ConnectionFunctions::ToolAuth(PublicStructers::client clientInfo, HINTERNET clientRequestSession)
{
	PublicStructers::ClientSession clientSessionInformation;
	clientSessionInformation.clientId = WinHttpRequests.SendPostData(JsonFunctions::CreateJsonLoginData(clientInfo), clientRequestSession, Encryptions::hexToAscii("2F746F6F6C2F617574682F6831"));
	if (clientSessionInformation.clientId.empty())
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif		
		exit(1);
	}
	clientSessionInformation.session = clientRequestSession;
	return clientSessionInformation;
}
