#pragma warning(disable : 4996)
#include <Windows.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <conio.h>
#include <process.h>
#include <vector>
using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;
using std::to_string;


int main() 
{
	string filename;
	cout << "Input filename:\n";
	cin >> filename;
	int numOfEnters;
	cout << "Input num of enters:\n";
	cin >> numOfEnters;
	int numOfSenders;
	cout << "Input num of senders:\n";
	cin >> numOfSenders;
	std::ofstream out;
	out.open(filename, fstream::binary);
	out.close();

	HANDLE hSemaphoreWrites = CreateSemaphore(NULL, numOfEnters, numOfEnters, (LPCWSTR)"SemaphoreWrites");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}


	HANDLE hSemaphoreReady = CreateSemaphore(NULL, 1 - numOfEnters, 1, (LPCWSTR)"SemaphoreReady");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hMutex = CreateMutex(NULL, FALSE, (LPCWSTR)"DemoMutex"); 
	if (hMutex == NULL)
	{
		cout << "Create mutex failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
	if (hEvent == NULL) return GetLastError();

	STARTUPINFO* si = new STARTUPINFO[numOfSenders];
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[numOfSenders];


	string arg = "Sender.exe " + filename;
	char args1[255];
	strcpy(args1, arg.c_str());
	cout << args1 << "\n";
	WCHAR* name_of_sender = new WCHAR[arg.size() + 1];
	mbstowcs(name_of_sender, args1, arg.size());
	name_of_sender[arg.size()] = 0;

	for (int i = 0; i < numOfSenders; i++) 
	{
		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));


		if (!CreateProcess(NULL, name_of_sender, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) 
		{
			cout << GetLastError();
			ExitProcess(0);
		}
	}


	vector<string> v;
	v.reserve(numOfEnters);
	WaitForSingleObject(hSemaphoreReady, INFINITE);


	while (true) 
	{
		cout << "Input 'read' to read file, or 'close' to stop the process:\n";
		string text;
		cin >> text;
		if (text == "close")
		{
			break;
		}
		else if (text == "read")
		{
			ifstream in;
			in.open(filename, fstream::binary);
			if (in.peek() == ifstream::traits_type::eof())
			{
				ResetEvent(hEvent);
				WaitForSingleObject(hEvent, INFINITE);
				ResetEvent(hEvent);
			}
			WaitForSingleObject(hMutex, INFINITE);
			string message;
			in.seekg(0, in.end);
			int pos = in.tellg() / 21;
			in.seekg(in.beg);
			v.clear();
			for (int i = 0; i < pos; ++i)
			{
				char mess[21];
				in.read(mess, 21);
				string s = mess;
				v.push_back(s);
			}
			in.close();
			ofstream out;
			out.open(filename, ofstream::binary | ofstream::out | ofstream::trunc);
			for (int i = 1; i < v.size(); ++i)
			{
				char mess[21];
				strcpy(mess, v[i].c_str());
				out.write(mess, 21);
			}
			out.close();
			cout << v[0] << "\n";
			ReleaseSemaphore(hSemaphoreWrites, 1, NULL);
			ReleaseMutex(hMutex);
		}
		else cout << "Input error. Try again.\n";
	}

	//// закрываем дескриптор мьютекса 
	//CloseHandle(hMutex);
	//// ждем пока дочерний процесс закончит работу 
	//WaitForSingleObject(pi.hProcess, INFINITE);
	//// закрываем дескрипторы дочернего процесса в текущем процессе 
	//CloseHandle(pi.hThread);
	//CloseHandle(pi.hProcess);


	for (int i = 0; i < numOfSenders; i++) 
	{
		TerminateProcess(pi[i].hProcess, 0);
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}
	CloseHandle(hMutex);
	return 0;
}