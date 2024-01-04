#pragma warning(disable: 4996)
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using std::cin;
using std::cout;
using std::ofstream;
using std::string;
int main(int argc, char* argv[]) 
{
	HANDLE hSemaphoreWrites = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"SemaphoreWrites");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hSemaphoreReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"SemaphoreReady");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, (LPCWSTR)"DemoMutex");
	if (hMutex == NULL)
	{
		cout << "Open mutex failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
	if (hEvent == NULL) return GetLastError();

	string s = argv[1];
	cout << s << "\n";

	ofstream out;
	out.open(s, ofstream::binary | std::ios_base::app);
	if(!out.is_open()) cout << "File not found.\n";
	ReleaseSemaphore(hSemaphoreReady, 1, NULL);
	string message;
	while (true) 
	{
		bool q = true;
		cout << "Input 'send' to send your message or input 'close' to stop:\n";
		cin >> message;
		if (message == "close")
		{
			break;
		}
		else if (message == "send")
		{

			while (q)
			{
				WaitForSingleObject(hSemaphoreWrites, INFINITE);
				cout << "Input your message (it must not be more than 20 character):\n";
				cin >> message;
				if (message.size() > 20)
				{
					cout << "The message must not be more than 20 character. Try again.\n";
					ReleaseSemaphore(hSemaphoreWrites, 1, NULL);
				}
				else q = false;
			}
			
			char mess[21];
			strcpy(mess, message.c_str());
			WaitForSingleObject(hMutex, INFINITE);

			if (!out.is_open()) out.open(s, ofstream::binary | std::ios_base::app);
			out.write(mess, 21);
			out.close();
			ReleaseMutex(hMutex);
			SetEvent(hEvent);
		}
		else cout << "Input error. Try again.\n";
	}
	CloseHandle(hMutex);
	return 0;
}