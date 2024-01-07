#include <iostream>
#include <Windows.h> 
using std::cin;
using std::cout;
HANDLE st;
int n;
int markerCount;
CRITICAL_SECTION criticalSection;
const int THREAD_DELAY = 5;

struct numsThread
{
	int* arr;
	int num;
	HANDLE stop = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE* event = new HANDLE[2];
};

DWORD WINAPI marker(LPVOID _arrF)
{
	WaitForSingleObject(st, INFINITE);
	numsThread arrF = *(static_cast<numsThread*>(_arrF));
	srand(arrF.num);
	bool check = false;
	int count = 0;
	while (!check)
	{
		int temp = rand();
		temp = temp % n;
		EnterCriticalSection(&criticalSection);
		if (arrF.arr[temp] == 0)
		{
			Sleep(THREAD_DELAY);
			arrF.arr[temp] = arrF.num;
			Sleep(THREAD_DELAY);
			count += 1;
			LeaveCriticalSection(&criticalSection);
		}
		else
		{
			cout << arrF.num << " " << count << " " << temp << "\n";
			LeaveCriticalSection(&criticalSection);
			SetEvent(arrF.stop);
			int k = WaitForMultipleObjects(2, arrF.event, FALSE, INFINITE) - WAIT_OBJECT_0;
			if (k == 0)
			{
				check = true;
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		if (arrF.arr[i] == arrF.num)
		{
			arrF.arr[i] = 0;
		}
	}
	return 0;
}


int main()
{
	InitializeCriticalSection(&criticalSection);
	st = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == st) return  GetLastError();
	int* arr;
	DWORD* dwThread;
	HANDLE* hThread;
	cout << "Enter size:\n";
	cin >> n;
	arr = new int[n];
	for (int i = 0; i < n; i++)
	{
		arr[i] = 0;
	}
	cout << "Enter amount of markets:\n";
	cin >> markerCount;
	hThread = new HANDLE[markerCount];
	dwThread = new DWORD[markerCount];
	numsThread* arrF = new numsThread[markerCount];
	bool* check = new bool[markerCount];
	HANDLE* stop = new HANDLE[markerCount];
	for (int i = 0; i < markerCount; i++)
	{
		arrF[i].arr = arr;
		arrF[i].num = i + 1;
		stop[i] = arrF[i].stop;
		arrF[i].event[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == arrF[i].event[0]) return GetLastError();
		arrF[i].event[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == arrF[i].event[1]) return GetLastError();
		hThread[i] = CreateThread(NULL, 0, marker, static_cast<LPVOID>(&arrF[i]), 0, &dwThread[i]);
		if (NULL == hThread[i]) return GetLastError();
		check[i] = false;
	}
	SetEvent(st);
	bool quit = true;
	while (quit)
	{
		quit = false;
		int index;
		WaitForMultipleObjects(markerCount, stop, TRUE, INFINITE);
		for (int i = 0; i < n; i++)
		{
			cout << arr[i] << " ";
		}
		cout << "\n";
		cout << "Enter the number of the thread to be completed: ";
		cin >> index;
		--index;
		if (index >= markerCount || index < 0)
		{
			cout << "Error input.\n";
		}
		else if (check[index])
		{
			cout << "This thread was ended.\n";
		}
		else
		{
			SetEvent(arrF[index].event[0]); //end the process
			WaitForSingleObject(hThread[index], INFINITE);
			for (int i = 0; i < n; i++)
			{
				cout << arr[i] << " ";
			}
			cout << "\n";
			check[index] = true;
		}

		for (int i = 0; i < markerCount; i++)
		{
			if (!check[i])
			{
				ResetEvent(arrF[i].stop);
				SetEvent(arrF[i].event[1]); //continue the process
				quit = true;
			}
		}
	}
	for (int i = 0; i < markerCount; i++)
	{
		CloseHandle(hThread[i]);
		CloseHandle(stop[i]);
		CloseHandle(arrF[i].event[0]);
		CloseHandle(arrF[i].event[1]);
	}
	CloseHandle(st);
	DeleteCriticalSection(&criticalSection);

	delete[] arr;
	delete[] hThread;
	delete[] dwThread;
	delete[] arrF;
	delete[] check;
	delete[] stop;

	return 0;
}
