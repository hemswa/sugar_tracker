#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <Python.h> // For embedding Python

// Structure to hold sugar details
struct SugarReading {
    std::string date;
    std::string time;
    int sugar_level;
    bool is_fasting;
};

// Function to get the current date and time
void getCurrentDateTime(std::string &date, std::string &time) {
    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);

    char date_buffer[11];
    char time_buffer[9];

    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", local_time);
    std::strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", local_time);

    date = date_buffer;
    time = time_buffer;
}

// Function to save readings to a file
void saveReading(const SugarReading &reading, const std::string &filename) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << reading.date << "," << reading.time << "," << reading.sugar_level << "," << reading.is_fasting << "\n";
        file.close();
    } else {
        std::cerr << "Error opening file for writing." << std::endl;
    }
}

// Function to load readings from a file
std::vector<SugarReading> loadReadings(const std::string &filename) {
    std::vector<SugarReading> readings;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        SugarReading reading;
        std::string fasting;

        std::getline(ss, reading.date, ',');
        std::getline(ss, reading.time, ',');
        ss >> reading.sugar_level;
        ss.ignore(); // Ignore the comma
        std::getline(ss, fasting);

        reading.is_fasting = (fasting == "1");
        readings.push_back(reading);
    }

    return readings;
}

// Function to generate graphs using Python
void generateGraph(const std::vector<SugarReading> &readings) {
    Py_Initialize(); // Start Python interpreter

    try {
        // Extract data for Python
        std::string fasting_indices = "[";
        std::string non_fasting_indices = "[";
        std::string fasting_levels = "[";
        std::string non_fasting_levels = "[";

        for (size_t i = 0; i < readings.size(); ++i) {
            if (readings[i].is_fasting) {
                fasting_indices += std::to_string(i) + ",";
                fasting_levels += std::to_string(readings[i].sugar_level) + ",";
            } else {
                non_fasting_indices += std::to_string(i) + ",";
                non_fasting_levels += std::to_string(readings[i].sugar_level) + ",";
            }
        }

        if (fasting_indices.back() == ',') fasting_indices.pop_back();
        if (fasting_levels.back() == ',') fasting_levels.pop_back();
        if (non_fasting_indices.back() == ',') non_fasting_indices.pop_back();
        if (non_fasting_levels.back() == ',') non_fasting_levels.pop_back();

        fasting_indices += "]";
        fasting_levels += "]";
        non_fasting_indices += "]";
        non_fasting_levels += "]";

        // Python code
        std::string python_code = R"(
import matplotlib.pyplot as plt

fasting_indices = )" + fasting_indices + R"(
fasting_levels = )" + fasting_levels + R"(
non_fasting_indices = )" + non_fasting_indices + R"(
non_fasting_levels = )" + non_fasting_levels + R"(

plt.figure()
plt.plot(fasting_indices, fasting_levels, 'b-o', label='Fasting')
plt.plot(non_fasting_indices, non_fasting_levels, 'r-o', label='Non-Fasting')
plt.legend()
plt.title('Blood Sugar Readings')
plt.xlabel('Reading Index')
plt.ylabel('Sugar Level')
plt.show()
)";

        PyRun_SimpleString(python_code.c_str());
    } catch (...) {
        std::cerr << "An error occurred while generating the graph.\n";
    }

    Py_Finalize(); // End Python interpreter
}

int main() {
    const std::string filename = "sugar_readings.csv";
    std::vector<SugarReading> readings;
    int choice;

    do {
        std::cout << "\nBlood Sugar Tracker\n";
        std::cout << "1. Add Sugar Reading\n";
        std::cout << "2. Generate Graph\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            SugarReading reading;
            std::string fasting_input;

            getCurrentDateTime(reading.date, reading.time);

            std::cout << "Enter sugar level: ";
            std::cin >> reading.sugar_level;

            std::cout << "Fasting (yes/no)? ";
            std::cin >> fasting_input;
            reading.is_fasting = (fasting_input == "yes" || fasting_input == "Yes");

            readings.push_back(reading);
            saveReading(reading, filename);

            std::cout << "Reading added successfully!\n";
        } else if (choice == 2) {
            readings = loadReadings(filename);
            if (readings.empty()) {
                std::cout << "No readings available to generate a graph.\n";
            } else {
                generateGraph(readings);
            }
        }
    } while (choice != 3);

    return 0;
}
