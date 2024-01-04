#include <iostream>
#include <fstream>
#include <string>

struct Employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::cerr << "Íåêîððåêòíûé òèï" << std::endl;
        return 1;
    }

    const char* inputFilename = argv[1];
    const char* reportFilename = argv[2];
    double hourlyRate = std::stod(argv[3]);

    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Íå óäàëîñü îòêðûòü âõîäíîé ôàéë." << std::endl;
        return 1;
    }

    std::ofstream reportFile(reportFilename);
    if (!reportFile) {
        std::cerr << "Íå óäàëîñü îòêðûòü ôàéë îò÷åòà." << std::endl;
        return 1;
    }
    reportFile << "The report of the file \"" << inputFilename << "\"" << std::endl;

    Employee employee;
    while (inFile.read(reinterpret_cast<char*>(&employee), sizeof(employee))) {
        double earnings = employee.hours * hourlyRate;
        reportFile << employee.num << " \t " << employee.name << " \t " << employee.hours << " \t " << earnings << std::endl;
    }

    inFile.close();
    reportFile.close();
    return 0;
}