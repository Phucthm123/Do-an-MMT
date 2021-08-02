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
#include <direct.h>
#include "rapidjson/document.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "Winhttp.lib")


using namespace rapidjson;

void ToUpper(std::string& name)
{
	for (int i = 0; i < name.length(); i++)
	{
		if (name[i] >= 97 && name[i] <= 122)
		{
			name[i] -= 32;
		}
	}
}

void safeToFile(std::string time, std::string typesOfBank, std::string data) {
	_mkdir(typesOfBank.c_str());
	std::string filename = typesOfBank + "\\" + time + ".txt";
	std::fstream fout(filename, std::ios::out);
	fout << data;
	fout.close();
}

std::wstring getAPI(HINTERNET a)
{
	std::wstring link = L"/api/request_api_key?scope=exchange_rate";
	std::wstring temp = L"";
	HINTERNET request = WinHttpOpenRequest(a, L"GET", link.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
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
					temp += std::wstring(temp1.begin(), temp1.end());
				}

				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (!result) std::cout << "Error has occurred: " << GetLastError() << "\n";

	if (request) WinHttpCloseHandle(request);

	int start = 0, end = 0;
	for (int i = 0; i < 3; i++)
	{
		start = temp.find(L"\"",start+1);
	}
	end = temp.length();
	wchar_t* result1 = new wchar_t[end - start + 1];
	temp.copy(result1, end - start - 4, start  + 1 );
	result1[end - start] = L'\0';

	return result1;
}

void GetDataFromAPI(HINTERNET a, std::wstring link, std::wstring API_KEY, std::string bankName)
{

	link += API_KEY;
	HINTERNET request = WinHttpOpenRequest(a, L"GET", link.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	bool result = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	bool response = WinHttpReceiveResponse(request, NULL);

	DWORD dwSize = 0;
	DWORD dwDownloadedd = 0;
	char* pszOutBuffer;
	std::string data;

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
					data += pszOutBuffer;
				}

				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (!result) std::cout << "Error has occurred: " << GetLastError() << "\n";

	if (request) WinHttpCloseHandle(request);

	//to save data by day
	struct tm newtime;
	time_t present = time(0);
	localtime_s(&newtime, &present);

	std::string TIME = "";

	TIME += newtime.tm_mday > 9 ? std::to_string(newtime.tm_mday) : "0" + std::to_string(newtime.tm_mday);
	TIME += newtime.tm_mday > 9 ? std::to_string(1 + newtime.tm_mon) : "0" + std::to_string(1 + newtime.tm_mon);
	TIME += std::to_string(1900 + newtime.tm_year);

	safeToFile(TIME, bankName, data);

}

void readData(std::string bankName, std::string time,std::string money)
{
	ToUpper(bankName);
	ToUpper(money);

	std::fstream fin(bankName + "\\" + time + ".txt", std::ios::in);
	if (fin.fail()) {
		std::cout << "Can't onpen !"; 
		return; 
	}

	std::string data;
	getline(fin, data);

	Document d;
	d.Parse(data.c_str());

	const Value& array = d["results"];

	for (SizeType i = 0; i < array.Size(); i++)
	{
		if (array[i]["currency"].GetString() == money)
		{
			std::cout << "\n";
			std::cout << "Currency: " << array[i]["currency"].GetString() << "\n";
			std::cout << "Buy_cash: " << array[i]["buy_cash"].GetDouble() << "\n";
			std::cout << "Buy_transfer: " << array[i]["buy_transfer"].GetDouble() << "\n";
			std::cout << "Sell: " << array[i]["sell"].GetDouble() << "\n\n";
			return;
		}
	}

	std::cout << "Can't find !" << "\n";

	fin.close();
}

int main()
{
	HINTERNET session = NULL, connect = NULL;
	session = WinHttpOpen(L"WinHTTP application/json HTTP/2.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session)
	{
		connect = WinHttpConnect(session, L"vapi.vnappmob.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

	std::wstring API = getAPI(connect);

	//------------------------------------------------------------------------------------------------------------------------------

	// SAVE DATA AS JSON FILE FOR SERVER TO TAKE
	//while (true)
	//{
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/vcb?api_key=", API,"VIETCOMBANK");
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/ctg?api_key=", API,"VIETINBANK");
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/tcb?api_key=", API,"TECHCOMBANK");
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/bid?api_key=", API,"BIDV");
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/stb?api_key=", API,"SACOMBANK");
		GetDataFromAPI(connect, L"//api/v2/exchange_rate/sbv?api_key=", API,"SBV");
		std::cout << "UPDATE DATA DONE!" << "\n\n";
		//Sleep(1800000);
	//}
		
		std::string bank, date, money;
		std::cout << "Plz choose your bank: ";
		std::cin >> bank;
		std::cout << "Plz enter a day (dd-mm-yyyy): ";
		std::cin >> date;
		std::cout << "Plz enter the type of money: ";
		std::cin >> money;

		readData(bank,date,money);

	if (session) WinHttpCloseHandle(session);
	if (connect) WinHttpCloseHandle(connect);
	return 0;
}

