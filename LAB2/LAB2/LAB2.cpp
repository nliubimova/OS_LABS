#include <iostream>
#include <thread>
#include<vector>
#include<chrono>
#include<algorithm>
#include<windows.h>

using namespace std;

CRITICAL_SECTION cs;

void findMinAndMax(int& min, int& max, vector<int>& vec) {

	min = vec[0];
	max = vec[0];
	for (int i = 1; i < vec.size(); i++) {
		if (min > vec[i]) {
			min = vec[i];
		}

		this_thread::sleep_for(chrono::milliseconds(7));

		if (max < vec[i]) {
			max = vec[i];
		}

		this_thread::sleep_for(chrono::milliseconds(7));
	}

	EnterCriticalSection(&cs);
	cout << "minimum is " << min << "\n";
	cout << "maximum is " << max << "\n";
	LeaveCriticalSection(&cs);
}

void findAvarage(int& average, vector<int>& vec) {
	int sum = 0;
	for (int i = 0; i < vec.size(); i++) {
		sum += vec[i];
		this_thread::sleep_for(chrono::milliseconds(12));
	}

	average = sum / vec.size();
	EnterCriticalSection(&cs);
	cout << "average is " << average << "\n";
	LeaveCriticalSection(&cs);
}

int main() {

	vector<int> numbers;
	int size = 0;
	int min, max = 0;
	int average = 0;
	int num = 0;
	InitializeCriticalSection(&cs);

	cout << "Enter the array size : ";
	cin >> size;

	for (int i = 1; i <= size; i++) {
		cout << " Enter the element with number " << i << " : ";
		cin >> num;
		numbers.push_back(num);
	}

	thread minMaxThread(findMinAndMax, ref(min), ref(max), ref(numbers));
	thread avarageThread(findAvarage, ref(average), ref(numbers));

	avarageThread.join();
	minMaxThread.join();
	DeleteCriticalSection(&cs);

	cout << "Input array : ";
	for (int i = 0; i < numbers.size(); i++) {
		cout << numbers[i] << " ";
	}
	cout << "\n";

	replace(numbers.begin(), numbers.end(), min, average);
	replace(numbers.begin(), numbers.end(), max, average);

	cout << "Result array : ";
	for (int i = 0; i < numbers.size(); i++) {
		cout << numbers[i] << " ";
	}

	return 0;
}