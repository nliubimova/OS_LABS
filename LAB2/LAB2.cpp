#include <windows.h>
#include <iostream>
#include <iomanip>

const int THREAD_DELAY_MIN_MAX = 7;
const int THREAD_DELAY_AVERAGE = 12;

struct Array
{
	double* a;
	int size;
	double average;
	double min_;
	double max_;
	Array (double* a_, int size_): a(a_), size(size_), average(0), min_(0), max_(0){}
	~Array() 
	{
		delete[] a;
	}
};

void Replace(Array* param)
{
	for (int i = 0; i < param->size; i++)
	{
		if (param->a[i] == param->max_ || param->a[i] == param->min_)
			param->a[i] = param->average;
	}
}

DWORD WINAPI Min_Max(LPVOID arr)
{
	std::cout << "Thread Min_Max is started.\n";
	double min_ = static_cast<Array*>(arr)->a[0];
	double max_ = min_;
	int index_min = 0, index_max = 0;
	for (int i = 1; i < static_cast<Array*>(arr)->size; i++)
	{
		if (static_cast<Array*>(arr)->a[i] > max_) 
		{
			max_ = static_cast<Array*>(arr)->a[i];
		}
		Sleep(THREAD_DELAY_MIN_MAX);
		if (static_cast<Array*>(arr)->a[i] < min_) 
		{
			min_ = static_cast<Array*>(arr)->a[i];
		}
		Sleep(THREAD_DELAY_MIN_MAX);
	}
	static_cast<Array*>(arr)->max_ = max_;
	static_cast<Array*>(arr)->min_ = min_;

	std::cout << "Min: " << min_ << '\n' << "Max: " << max_ << '\n' 
		<< "Thread Min_Max is finished.\n";
	return 0;
}

DWORD WINAPI Average(LPVOID arr)
{
	std::cout << "Thread Average is started.\n";
	double sum = 0;
	for (int i = 0; i < static_cast<Array*>(arr)->size; i++)
	{
		sum += static_cast<Array*>(arr)->a[i];
		Sleep(THREAD_DELAY_AVERAGE);
	}
	static_cast<Array*>(arr)->average = sum / static_cast<Array*>(arr)->size;
	

	std::cout << "Average: " << static_cast<Array*>(arr)->average << '\n' 
		<< "Thread Average is finished.\n";
	return 0;
}

int main()
{
	HANDLE hThread;
	DWORD IDThread;
	int n;
	std::cout << "Enter count of elements: ";
	std::cin >> n;
	double* a = new double[n];
	std::cout << "Enter elements:\n";
	for (int i = 0; i < n; ++i)
	{
		std::cin >> a[i];
	}
	auto param = new Array(a, n);
	hThread = CreateThread(NULL, 0, Min_Max, (LPVOID)param, 0, &IDThread);
	if (hThread == NULL)
		return GetLastError();
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	HANDLE hAverage;
	DWORD IDAverage;

	hAverage = CreateThread(NULL, 0, Average, (LPVOID)param, 0, &IDAverage);
	if (hAverage == NULL)
		return GetLastError();
	WaitForSingleObject(hAverage, INFINITE);
	CloseHandle(hAverage);

	std::cout << "New array: ";
	Replace(param);
	for (int i = 0; i < param->size; i++)
	{
		std::cout << std::setprecision(3) << param->a[i] << " ";
	}
	std::cout << '\n';

	delete[] a;

	return 0;
}
