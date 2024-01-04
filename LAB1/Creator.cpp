#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>

struct Employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Something wrong with parametars" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int numRecords = std::stoi(argv[2]);


    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Can not open the bin file" << std::endl;
        return 1;
    }

    Employee employee;
    for (int i = 0; i < numRecords; ++i) {
        std::cout << "Enter data for writing: " << i + 1 << std::endl;
        std::cout << "Number employee: ";
        std::cin >> employee.num;
        std::cout << "Name of the employee: ";
        std::cin >> employee.name;
        std::cout << "Amount of hours: ";
        std::cin >> employee.hours;
        outFile.write(reinterpret_cast<char*>(&employee), sizeof(employee));
    }
    outFile.close();
    return 0;
}