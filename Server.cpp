#include <windows.h>
#include <iostream>
#include <string>
#include "Header.h";
#include <fstream>;

#pragma warning (disable : 4996)
using namespace std;

struct employee
{
	int id;
	char name[10];
	double hours;
};
struct Command
{
	int id;
	int request;

	std::string convert()
	{
		std::string str = std::to_string(request) + " " + std::to_string(id);
		return  str;
	};
	Command()
	{
		this->id = -1;
		this->request = 0;
	}
};
std::string semaphoreNameBase = "Semaphore";
std::string path = "C:\\OS\\Lab5\\Lab5\\bin.bin";
char c;
char* tempStr;
SECURITY_ATTRIBUTES sa; 
SECURITY_DESCRIPTOR sd; 
STARTUPINFO startupInfo;
DWORD dwBytesRead; 
char OutMessage[80]; 
char InMessage[80];
DWORD dwBytesWrite; 
int processAmount;
PROCESS_INFORMATION processInfo;
HANDLE* processHandles;
const char* pipeNameBase = "\\\\.\\pipe\\demo_pipe";
HANDLE* hNamedPipes;
HANDLE* hThreads;
std::string fileName;
ifstream IFile;
ofstream OFile;
int notesAmmount;
HANDLE* hSemaphores;
int index;

int creatingSemaphores()
{
	hSemaphores = new HANDLE[notesAmmount];
	for (int i = 0; i < notesAmmount; i++)
	{
		std::string str = semaphoreNameBase + std::to_string(i);
		hSemaphores[i] = CreateSemaphore(&sa, processAmount, processAmount, str.c_str());
		if (hSemaphores[i] == NULL)
		{
			std::cout << "Error Creating the Semaphore\n";
			return 1;
		}
	}
	return 0;
};

int prepearingFiles()
{
	std::cout << "Enter the Input file name: ";
	std::cin >> fileName;
	OFile = ofstream(fileName, std::ios::binary);
	std::cout << "Enter the ammount of employees: ";
	std::cin >> notesAmmount;
	for (int i = 0; i < notesAmmount; i++)
	{
		employee temp = employee();
		temp.id = i;
		std::cout << "Enter the Data\n";
		std::cout << "ID: " << i << "\n";
		std::cout << "Name: ";
		std::cin >> temp.name;
		std::cout << "Hours: ";
		std::cin >> temp.hours;
		OFile.write((char*)&temp, sizeof(employee));
	}
	OFile.close();
	IFile = ifstream(fileName, std::ios::binary);
	for (int i = 0; i < notesAmmount; i++)
	{
		employee temp = employee();
		IFile.read((char*)&temp, sizeof(employee));
		std::cout << temp.id << " " << temp.hours << " " << temp.name << "\n";
	}
	IFile.close();
	return 0;
}

int creatingPipe()
{
	hNamedPipes = new HANDLE[processAmount];
	for (int i = 0; i < processAmount; i++)
	{
		std::string pipeName = pipeNameBase + std::to_string(i);
		hNamedPipes[i] = CreateNamedPipe(
			pipeName.c_str(), 
			PIPE_ACCESS_DUPLEX, 
			PIPE_TYPE_MESSAGE | PIPE_WAIT, 
			PIPE_UNLIMITED_INSTANCES,
			0, 
			0, 
			INFINITE, 
			&sa 
		);

		if (hNamedPipes[i] == INVALID_HANDLE_VALUE)
		{
			cerr << "Creation of the named pipe failed." << endl
				<< "The last error code: " << GetLastError() << endl;
			std::cout << "Press any char to finish server: ";
			std::cin >> c;
			return 1;
		}
	}
	return 0;
}

int creatingProcess()
{
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	processHandles = new HANDLE[processAmount];
	for (int i = 0; i < processAmount; i++)
	{
		std::string command = "Client.exe ";
		command += std::to_string(i);

		if (!CreateProcess(NULL, &command[0], &sa, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
		{
			std::cerr << "Failed to create a Client process." << std::endl;
			std::cerr << GetLastError();
			return 1;
		}
		processHandles[i] = processInfo.hProcess;
	}

	return 0;
}

int waitingClient(HANDLE hNamedPipe)
{
	if (!ConnectNamedPipe(

		hNamedPipe, 
		(LPOVERLAPPED)NULL 
	))
	{
		cerr << "The connection failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		std::cout << "Press any char to finish the server: ";
		std::cin >> c;
		return 1;
	}
	return 0;
}

int creatingThreads()
{
	DWORD* IDThreads = new DWORD[processAmount];
	hThreads = new HANDLE[processAmount];
	for (int i = 0; i < processAmount; i++)
	{
		hThreads[i] = CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)processingThread, (void*)i, 0, &IDThreads[i]);
	}
	return 0;
}

int readingMessage(HANDLE hNamedPipe, char* inMessage)
{

	if (!ReadFile(

		hNamedPipe, 
		inMessage, 
		sizeof(InMessage), 
		&dwBytesRead, 
		(LPOVERLAPPED)NULL 
	))
	{
		if (GetLastError() == 109)
		{
			std::cout << "One process Closed\n";
			return 1;
		}
		cerr << "Data reading from the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		std::cout << "Press any char to finish the server: ";
		std::cin >> c;
		return 1;
	}
	return 0;
}

int writingMessage(HANDLE hNamedPipe)
{
	std::cout << "IN Message writing\n";
	std::cout << OutMessage << "\n";
	if (!WriteFile(
		hNamedPipe, 
		OutMessage, 
		sizeof(OutMessage), 
		&dwBytesWrite, 
		(LPOVERLAPPED)NULL 
	))
	{
		cerr << "Data writing to the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		std::cout << "Press any char to finish the server: ";
		std::cin >> c;
		return 1;
	}
	std::cout << "Message writed\n";
	return 0;
}

int requestProcessing(char* inMessage)
{
	std::cout << "In request Processing\n";
	std::string str = inMessage;

	switch (inMessage[0])
	{
	case '1':
		str = str.substr(2);
		index = stoi(str);
		std::cout << "In case 1, index = " << index << " in Meesage = " << str << "\n";
		return 0;
	case '2':
		str = str.substr(2);
		index = stoi(str);
		std::cout << "In case 2, index = " << index << " " << str << "\n";
		return 2;
	default:
		std::cout << "Wrong Request in Server\n";
		return 1;
	}
}

int readRequest()
{
	employee temp = employee();
	ifstream IFile;
	IFile.open(fileName, std::ios::binary);
	IFile.seekg(index * sizeof(employee));
	IFile.read((char*)&temp, sizeof(employee));
	std::string str = "ID: " + std::to_string(temp.id) + " Name: " + temp.name + " Hours: " + std::to_string(temp.hours);
	strcpy(OutMessage, str.c_str());

	IFile.close();
	return 0;
}

int modifyRequest(HANDLE hPipe)
{

	//std::cout << "Readed mesage from Client: " << InMessage << "\n";
	std::string str = InMessage;
	std::string strId = str.substr(0, str.find(' '));
	//std::cout << "Parsed strId: " << strId << "\n";
	std::string strNameHours = str.substr(str.find(' ') + 1);
	//std::cout << "Parsed strNameHours: " << strNameHours << "\n";
	std::string strName = strNameHours.substr(0, strNameHours.find(' '));
	//std::cout << "Parsed strName: " << strName << "\n";

	std::string strHours = strNameHours.substr(strNameHours.find(' ') + 1);

	std::cout << "Parsed inMessage: " << strId + " " + strName + " " + strHours + "\n";

	employee temp = employee();
	temp.id = stoi(strId);
	temp.hours = stod(strHours);
	strcpy(temp.name, strName.c_str());
	std::cout << "After copying in employee\n";
	/*if (OFile.fail()) {
		std::cerr << "Error opening file for modification. Details: " << strerror(errno) << std::endl;
		return 1;
	}*/
	if (!OFile.is_open())
	{
		std::cerr << "File is not open: " << strerror(errno) << "\n";
	}

	std::cout << "Id: " << temp.id << temp.name << temp.hours << "\n";
	std::cout << fileName << "\n";
	ofstream OFile;
	OFile = ofstream(fileName, std::ios::binary | std::ios::app);
	if (!OFile.is_open())
	{
		std::cerr << "File is not open: " << strerror(errno) << "\n";
	}
	std::cout << "After opening file\n";
	OFile.seekp(temp.id * sizeof(employee));
	OFile.write((char*)&temp, sizeof(employee));
	OFile.close();
	return 0;

}

int main()
{
	
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE; 

	
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);


	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.lpSecurityDescriptor = &sd;

	if (prepearingFiles() == 1)
		return 0;


	std::cout << "Enter the amount of procces Clients: ";
	std::cin >> processAmount;

	if (creatingPipe() == 1)
		return 0;

	if (creatingSemaphores() == 1)
		return 0;

	
	if (creatingProcess() == 1)
		return 0;

	if (creatingThreads() == 1)
		return 0;

	std::cout << "Connected\n";
	WaitForMultipleObjects(processAmount, hThreads, TRUE, INFINITE);
	std::cout << "CLosing the Threads\n";


	for (int i = 0; i < processAmount; i++)
	{
		CloseHandle(hNamedPipes[i]);
		CloseHandle(hSemaphores[i]);
	}
	std::cout << "Press any char to finish the server: ";
	std::cin >> c;

	return 0;
}