#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include <sstream>
#include <winhttp.h>
#include <fstream>
#include <sql.h>
#include <windows.data.json.h>
#include <ctime>
#include <sstream>
#include "rapidjson/document.h"


#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "Winhttp.lib")


using namespace rapidjson;


std::wstring getAPI(HINTERNET a)
{
	std::wstring link = L"/api/request_api_key?scope=exchange_rate";
	std::wstring temp = L"";
	HINTERNET request = WinHttpOpenRequest(a, L"GET", link.c_str() , NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	bool result = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	bool response = WinHttpReceiveResponse(request, NULL);

	DWORD dwSize = 0;
	DWORD dwDownloadedd = 0;
	char* pszOutBuffer;

	if (response)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(request, &dwSize))
			{
				std::cout << "Error in WinHttpQueryDataAvalable: " << GetLastError() << "\n";
			}

			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				std::cout << "Out of memory\n";
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!WinHttpReadData(request, (LPVOID)pszOutBuffer, dwSize, &dwDownloadedd))
				{
					std::cout << "Error in WinHttpReadData: " << GetLastError() << "\n";
				}
				else
				{
					//std::cout << pszOutBuffer << "\n";
					std::string temp1 = std::string(pszOutBuffer);
					temp = std::wstring(temp1.begin(), temp1.end());

					delete[] pszOutBuffer;
					if (request) WinHttpCloseHandle(request);
					return temp;
				}

				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (!result) std::cout << "Error has occurred: " << GetLastError() << "\n";

	if (request) WinHttpCloseHandle(request);
	
	return temp;
}

std::wstring GetDataFromAPI(HINTERNET a, std::wstring link, std::wstring API)
{
	std::wstring temp = link;
	link += API;
	HINTERNET request = WinHttpOpenRequest(a, L"GET", link.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	bool result = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	bool response = WinHttpReceiveResponse(request, NULL);
	
	DWORD dwSize = 0;
	DWORD dwDownloadedd = 0;
	char* pszOutBuffer;
	
	if (response)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(request, &dwSize))
			{
				std::cout << "Error in WinHttpQueryDataAvalable: " << GetLastError() << "\n";
			}

			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				std::cout << "Out of memory\n";
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!WinHttpReadData(request, (LPVOID)pszOutBuffer, dwSize, &dwDownloadedd))
				{
					std::cout << "Error in WinHttpReadData: " << GetLastError() << "\n";
				}
				else
				{
					std::string temp1 = std::string(pszOutBuffer);
					temp = std::wstring(temp1.begin(), temp1.end());
					break;
				}

				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}



	if (request) WinHttpCloseHandle(request);

	return temp;
}



int main()
{
	HINTERNET session = NULL, connect = NULL;
	session = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session)
	{
		connect = WinHttpConnect(session, L"vapi.vnappmob.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

	std::wstring API = getAPI(connect);
	std::string temp = std::string(API.begin(), API.end());
	const char* a = (char*)temp.c_str();

	Document d;
	d.Parse(a);
	temp = d["results"].GetString();
	API = std::wstring(temp.begin(), temp.end());

	//------------------------------------------------------------------------------------------------------------------------------

	HINTERNET session1 = NULL, connect1 = NULL;
	session1 = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session1)
	{
		connect1 = WinHttpConnect(session1, L"vapi.vnappmob.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
	}
	std::wstring temp2 = GetDataFromAPI(connect1, L"//api/v2/exchange_rate/vcb?api_key=", API);
	temp = std::string(temp2.begin(), temp2.end());
	a = (char*)temp.c_str();
	std::cout << a;

	return 0;

}


