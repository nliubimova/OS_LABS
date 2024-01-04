#include <Windows.h>
#include <iostream>

using namespace std;

int arraySize = 0;
int* arr = nullptr;
CRITICAL_SECTION cs;
HANDLE* handleThreads;
HANDLE* handleThreadsAreStarted;
HANDLE* handleThreadsAreStopped;
HANDLE* handleThreadsAreExited;
HANDLE handleMutex;

DWORD WINAPI marker(LPVOID threadIndex)
{
    WaitForSingleObject(handleThreadsAreStarted[(int)threadIndex], INFINITE);

    int markedNumbersCounter = 0;
    srand((int)threadIndex);

    while (true) {
        EnterCriticalSection(&cs);

        int randomNumber = rand() % arraySize;
        if (arr[randomNumber] == 0) {
            Sleep(5);
            arr[randomNumber] = (int)threadIndex + 1;
            markedNumbersCounter++;
            Sleep(5);
            LeaveCriticalSection(&cs);
        }
        else {
            cout << "\tNumber of threads: " << (int)threadIndex + 1 << "\n";
            cout << "\tAmount of marked element: " << markedNumbersCounter << "\n";
            cout << "\tIndex of an element that cannot be marked: " << randomNumber << "\n";
            cout << "\n";
            LeaveCriticalSection(&cs);

            SetEvent(handleThreadsAreStopped[(int)threadIndex]);
            ResetEvent(handleThreadsAreStarted[(int)threadIndex]);

            HANDLE handleThreadStartedExitedPair[]{ handleThreadsAreStarted[(int)threadIndex], handleThreadsAreExited[(int)threadIndex] };

            if (WaitForMultipleObjects(2, handleThreadStartedExitedPair, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(&cs);
                for (size_t i = 0; i < arraySize; i++) {
                    if (arr[i] == (int)threadIndex + 1) {
                        arr[i] = 0;
                    }
                }
                LeaveCriticalSection(&cs);

                ExitThread(NULL);
            }
            else {
                ResetEvent(handleThreadsAreStopped[(int)threadIndex]);
                continue;
            }
        }
    }
}

int main()
{

    int amountOfThreads = 0;
    cout << "Enter the size of the element array: ";
    cin >> arraySize;
    arr = new int[arraySize] {};
    cout << "Enter the number of threads : ";
    cin >> amountOfThreads;

    InitializeCriticalSection(&cs);

    handleThreads = new HANDLE[amountOfThreads];
    handleThreadsAreStarted = new HANDLE[amountOfThreads];
    handleThreadsAreStopped = new HANDLE[amountOfThreads];
    handleThreadsAreExited = new HANDLE[amountOfThreads];
    handleMutex = CreateMutex(NULL, FALSE, NULL);

    for (int i = 0; i < amountOfThreads; i++) {
        handleThreads[i] = CreateThread(NULL, 1, marker, (LPVOID)i, NULL, NULL);
        handleThreadsAreStarted[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        handleThreadsAreStopped[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        handleThreadsAreExited[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

    }

    for (int i = 0; i < amountOfThreads; i++) {
        SetEvent(handleThreadsAreStarted[i]);
    }

    int amount_of_completed_threads = 0;
    bool* is_thread_exited = new bool[amountOfThreads] {};
    while (amount_of_completed_threads < amountOfThreads) {
        WaitForMultipleObjects(amountOfThreads, handleThreadsAreStopped, TRUE, INFINITE);

        handleMutex = OpenMutex(NULL, FALSE, NULL);

        cout << "The resulting array: ";
        for (int i = 0; i < arraySize; i++) {
            cout << arr[i] << " ";
        }
        cout << "\n";

        ReleaseMutex(handleMutex);

        int stopMarkerId;
        cout << "Enter the number of the thread you want to stop:\n";
        cin >> stopMarkerId;
        stopMarkerId--;

        if (!is_thread_exited[stopMarkerId]) {
            amount_of_completed_threads++;
            is_thread_exited[stopMarkerId] = true;

            SetEvent(handleThreadsAreExited[stopMarkerId]);
            WaitForSingleObject(handleThreads[stopMarkerId], INFINITE);
            CloseHandle(handleThreads[stopMarkerId]);
            CloseHandle(handleThreadsAreExited[stopMarkerId]);
            CloseHandle(handleThreadsAreStarted[stopMarkerId]);
        }

        handleMutex = OpenMutex(NULL, FALSE, NULL);

        cout << "Array: ";
        for (int i = 0; i < arraySize; i++) {
            cout << arr[i] << " ";
        }
        cout << "\n";

        ReleaseMutex(handleMutex);

        for (int i = 0; i < amountOfThreads; i++) {
            if (!is_thread_exited[i]) {
                ResetEvent(handleThreadsAreStopped[i]);
                SetEvent(handleThreadsAreStarted[i]);
            }
        }
    }
    for (int i = 0; i < amountOfThreads; i++) {
        CloseHandle(handleThreadsAreStopped[i]);
    }
    DeleteCriticalSection(&cs);
    return 0;
}