#include <Windows.h>
#include <iostream>
#include <string>

#pragma warning (disable : 4996)

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


using namespace std;

char c; 
HANDLE hNamedPipe;
char machineName[80];
char pipeName[80];
char lpszOutMessage[80]; 
DWORD dwBytesWritten; 
char lpszInMessage[80]; 
DWORD dwBytesRead; 
int index;



int pipeConnecting()
{
	std::string pipeName = "\\\\.\\pipe\\demo_pipe" + std::to_string(index);
	hNamedPipe = CreateFile(
		pipeName.c_str(), 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		(LPSECURITY_ATTRIBUTES)NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE)NULL 
	);
	
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cerr << "Connection with the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any char to finish the client: ";
		cin >> c;
		return 1;
	}
	std::cout << "Connected\n";
	return 0;
}

int writingMessage()
{
	if (!WriteFile(
		hNamedPipe, 
		lpszOutMessage, 
		sizeof(lpszOutMessage), 
		&dwBytesWritten, 
		(LPOVERLAPPED)NULL 
	))
	{

		cerr << "Writing to the named pipe failed: " << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any char to finish the client: ";
		cin >> c;
		CloseHandle(hNamedPipe);
		return 1;
	}
	return 0;
}

int sendEmployee(employee& temp)
{
	std::string str = std::to_string(temp.id) + " " + temp.name + " " + std::to_string(temp.hours);

	strcpy(lpszOutMessage, str.c_str());
	if (!WriteFile(
		hNamedPipe,
		lpszOutMessage, 
		sizeof(lpszOutMessage), 
		&dwBytesWritten, 
		(LPOVERLAPPED)NULL 
	))
	{

	
		cerr << "Writing to the named pipe failed: " << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any char to finish the client: ";
		cin >> c;
		CloseHandle(hNamedPipe);
		return 1;
	}
	std::cout << "Message sent after working sendEmployee: " << str << "\n";
	return 0;
}

int readingMessage()
{
	if (!ReadFile(
		hNamedPipe, 
		lpszInMessage, 
		sizeof(lpszInMessage), 
		&dwBytesRead, 
		(LPOVERLAPPED)NULL 
	))
	{
	
		cerr << "Reading to the named pipe failed: " << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any char to finish the client: ";
		cin >> c;

		CloseHandle(hNamedPipe);
		return 1;
	}
	return 0;
}

int createRequest(const int num, const int id)
{
	Command command = Command();
	std::string	str = "";
	switch (num)
	{
	case 1:
		command.request = 1;
		command.id = id;
		str = command.convert();
		strcpy(lpszOutMessage, str.c_str());
		std::cout << "Converted command: " << command.convert() << "\n";
		std::cout << lpszOutMessage << "\n";
		break;
	case 2:
		command.request = 2;
		command.id = id;
		str = command.convert();
		strcpy(lpszOutMessage, str.c_str());
		std::cout << lpszOutMessage << "\n";
		break;
	default:
		std::cout << "Something went wrong\n";
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	index = atoi(argv[1]);

	if (pipeConnecting() == 1)
		return 0;

	while (true)
	{
	
		cout << "Enter the option: \n1. Modify\n2. Read\n3. Quit\nYour option: ";

		cin >> c;
		int id;
		switch (c)
		{
			
		case '1':
			std::cout << "Enter the id of Employee that you want to change: ";
			std::cin >> id;
			if (createRequest(1, id) == 1)
				return 0;
			if (writingMessage() == 1)
				return 0;
			std::cout << "Waiting for Server response...\n";
			if (readingMessage() == 1)
				return 0;
			employee temp = employee();
			temp.id = id;
			std::cout << "Enter the data, that you want to modify:\n ";
			std::cout << "ID: " << id << "\n";
			std::cout << "Name: ";
			std::cin >> temp.name;
			std::cout << "Hours: ";
			std::cin >> temp.hours;

			if (sendEmployee(temp) == 1)
				return 0;
			if (readingMessage() == 1)
				return 0;
			c = '0';
			std::cout << "now c: " << c << "\n";
			while (c != '1') {
				std::cout << "Enter 1 to confirm that you read information\n";
				cin >> c;
			}

			if (writingMessage() == 1)
				return 0;
			if (readingMessage() == 1)
				return 0;
			break;
			
		case '2':
			std::cout << "Enter the id of Employee that you want to read: ";
			std::cin >> id;

			cout << "Waiting Server response\n";
			if (createRequest(2, id) == 1)
				return 0;
			if (writingMessage() == 1)
				return 0;

			std::cout << "Waiting for Server response...\n";
			if (readingMessage() == 1)
				return 0;

			
			cout << "The client has received the following message from a server: "
				<< endl << "\t" << lpszInMessage << endl;
			c = '0';
			while (c != '1') {
				std::cout << "Enter 1 to confirm that you read information\n";
				cin >> c;
			}

			if (writingMessage() == 1)
				return 0;
			if (readingMessage() == 1)
				return 0;

			break;
		case '3':
			CloseHandle(hNamedPipe);
		
			cout << "Press any char to finish the client: ";
			cin >> c;
			return 0;
		default:
			cout << "Wrong choice, try again\n";
			break;
		}
	}
}