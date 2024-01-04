#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#pragma warning (disable : 4996)

struct Employee {
	int num;
	char name[10];
	double hours;
};


int main()
{

	Employee employee;
	std::string binaryFileName;

	int numRecords;

	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	ZeroMemory(&piApp, sizeof(piApp));

	std::cout << "Enter the name of Bin file: ";
	std::cin >> binaryFileName;
	std::cout << "Enter the amount of notes: ";
	std::cin >> numRecords;
	std::string str = std::to_string(numRecords);
	std::string createCommand = binaryFileName + " " + str;

	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, createCommand.c_str(), -1, nullptr, 0);
	wchar_t* wideString = new wchar_t[bufferSize];
	MultiByteToWideChar(CP_UTF8, 0, createCommand.c_str(), -1, wideString, bufferSize);
	std::wstring commandLine = L"Creator.exe " + std::wstring(wideString);

	if (!CreateProcess(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		DWORD error = GetLastError();
		LPVOID errorMessage;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error,
			0,
			(LPWSTR)&errorMessage,
			0,
			NULL
		);

		wprintf(L"Error: %s\n", (LPWSTR)errorMessage);
		LocalFree(errorMessage);
		std::cerr << "Error. Error code: " << error << std::endl;
		if (error == ERROR_FILE_NOT_FOUND) {
			std::cerr << "File not found: Creator.exe" << std::endl;
		}
		return 1;
	}

	WaitForSingleObject(piApp.hProcess, INFINITE);

	CloseHandle(piApp.hProcess);
	CloseHandle(piApp.hThread);

	std::cout << "The plot of the bin file: " << std::endl;

	std::ifstream in(binaryFileName, std::ios::binary);
	if (!in)
	{
		std::cout << "Something wrong";
	}
	for (int i = 0; i < numRecords; i++)
	{
		in.read(reinterpret_cast<char*>(&employee), sizeof(employee));
		std::cout << employee.num << "\t" << employee.name << "\t" << employee.hours << std::endl;
	}
	in.close();


	std::string reportFileName;
	double hourlyRate;

	std::cout << "Enter the reporter name: ";
	std::cin >> reportFileName;
	std::cout << "Enter the amount of money per hour: ";
	std::cin >> hourlyRate;

	std::string reportCommand = binaryFileName + " " + reportFileName + " " + std::to_string(hourlyRate);

	bufferSize = MultiByteToWideChar(CP_UTF8, 0, reportCommand.c_str(), -1, nullptr, 0);
	wideString = new wchar_t[bufferSize];
	MultiByteToWideChar(CP_UTF8, 0, reportCommand.c_str(), -1, wideString, bufferSize);
	commandLine = L"Reporter.exe " + std::wstring(wideString);

	if (!CreateProcess(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		std::cerr << "Error with Lunching Reporter(" << std::endl;
		return 1;
	}

	WaitForSingleObject(piApp.hProcess, INFINITE);

	CloseHandle(piApp.hProcess);
	CloseHandle(piApp.hThread);

	std::cout << "The reported file: " << std::endl;
	std::ifstream inf(reportFileName, std::ios::in, std::ios::trunc);
	std::string line;
	while (std::getline(inf, line)) {
		std::cout << line << std::endl;
	}

	inf.close();
	return 0;
}