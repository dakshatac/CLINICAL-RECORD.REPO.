#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>
#include <shared_mutex>

// Define a structure for a Patient Record
struct PatientRecord {
    int id;
    std::string name;
    int age;
    std::string condition;

    // Default constructor
    PatientRecord() : id(0), name(""), age(0), condition("") {}

    // Parameterized constructor
    PatientRecord(int id, std::string name, int age, std::string condition)
        : id(id), name(name), age(age), condition(condition) {}

    // Copy constructor
    PatientRecord(const PatientRecord& other)
        : id(other.id), name(other.name), age(other.age), condition(other.condition) {}

    // Move constructor
    PatientRecord(PatientRecord&& other) noexcept
        : id(other.id), name(std::move(other.name)), age(other.age), condition(std::move(other.condition)) {}

    // Copy assignment operator
    PatientRecord& operator=(const PatientRecord& other) {
        if (this != &other) {
            id = other.id;
            name = other.name;
            age = other.age;
            condition = other.condition;
        }
        return *this;
    }

    // Move assignment operator
    PatientRecord& operator=(PatientRecord&& other) noexcept {
        if (this != &other) {
            id = other.id;
            name = std::move(other.name);
            age = other.age;
            condition = std::move(other.condition);
        }
        return *this;
    }
};

// Hash table for quick access by ID
std::unordered_map<int, PatientRecord> recordMap;

// Vector to maintain a sorted list of records by age
std::vector<PatientRecord> recordList;

// Mutex for thread-safe operations
std::shared_mutex recordMutex;

// Function to add a patient record with input validation
void addRecord(int id, const std::string& name, int age, const std::string& condition) {
    if (id <= 0 || name.empty() || age <= 0 || condition.empty()) {
        std::cerr << "Invalid input. Record not added.\n";
        return;
    }

    std::unique_lock<std::shared_mutex> lock(recordMutex);

    // Check if the record already exists
    if (recordMap.find(id) != recordMap.end()) {
        std::cerr << "Record with this ID already exists.\n";
        return;
    }

    PatientRecord newRecord(id, name, age, condition);
    recordMap[id] = newRecord;
    recordList.push_back(newRecord);

    // Sort the vector by age for efficient retrieval
    std::sort(recordList.begin(), recordList.end(), [](const PatientRecord& a, const PatientRecord& b) {
        return a.age < b.age;
    });

    std::cout << "Record added successfully.\n";
}

// Function to retrieve a patient record by ID
void retrieveRecordByID(int id) {
    std::shared_lock<std::shared_mutex> lock(recordMutex);

    if (recordMap.find(id) != recordMap.end()) {
        PatientRecord record = recordMap[id];
        std::cout << "ID: " << record.id << ", Name: " << record.name
                  << ", Age: " << record.age << ", Condition: " << record.condition << "\n";
    } else {
        std::cout << "Record not found.\n";
    }
}

// Function to retrieve all records in sorted order by age
void retrieveRecordsSortedByAge() {
    std::shared_lock<std::shared_mutex> lock(recordMutex);

    for (const auto& record : recordList) {
        std::cout << "ID: " << record.id << ", Name: " << record.name
                  << ", Age: " << record.age << ", Condition: " << record.condition << "\n";
    }
}

// Function to delete a record by ID
void deleteRecord(int id) {
    std::unique_lock<std::shared_mutex> lock(recordMutex);

    if (recordMap.find(id) != recordMap.end()) {
        recordMap.erase(id);

        // Remove from the vector
        recordList.erase(std::remove_if(recordList.begin(), recordList.end(), [id](const PatientRecord& record) {
            return record.id == id;
        }), recordList.end());

        std::cout << "Record deleted successfully.\n";
    } else {
        std::cout << "Record not found.\n";
    }
}

// Function to update a record by ID
void updateRecord(int id, const std::string& name, int age, const std::string& condition) {
    if (id <= 0 || name.empty() || age <= 0 || condition.empty()) {
        std::cerr << "Invalid input. Record not updated.\n";
        return;
    }

    std::unique_lock<std::shared_mutex> lock(recordMutex);

    if (recordMap.find(id) != recordMap.end()) {
        PatientRecord& record = recordMap[id];
        record.name = name;
        record.age = age;
        record.condition = condition;

        // Update the record in the vector
        auto it = std::find_if(recordList.begin(), recordList.end(), [id](const PatientRecord& r) {
            return r.id == id;
        });

        if (it != recordList.end()) {
            *it = record;

            // Re-sort the vector if the age has changed
            std::sort(recordList.begin(), recordList.end(), [](const PatientRecord& a, const PatientRecord& b) {
                return a.age < b.age;
            });
        }

        std::cout << "Record updated successfully.\n";
    } else {
        std::cout << "Record not found.\n";
    }
}

int main() {
    int choice;
    do {
        std::cout << "\nPatient Record Database\n";
        std::cout << "1. Add Record\n";
        std::cout << "2. Retrieve Record by ID\n";
        std::cout << "3. Retrieve All Records Sorted by Age\n";
        std::cout << "4. Update Record\n";
        std::cout << "5. Delete Record\n";
        std::cout << "6. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                int id, age;
                std::string name, condition;
                std::cout << "Enter ID: ";
                std::cin >> id;
                std::cin.ignore(); // Clear the newline character from input buffer
                std::cout << "Enter Name: ";
                std::getline(std::cin, name);
                std::cout << "Enter Age: ";
                std::cin >> age;
                std::cin.ignore(); // Clear the newline character from input buffer
                std::cout << "Enter Condition: ";
                std::getline(std::cin, condition);
                addRecord(id, name, age, condition);
                break;
            }
            case 2: {
                int id;
                std::cout << "Enter ID: ";
                std::cin >> id;
                retrieveRecordByID(id);
                break;
            }
            case 3:
                retrieveRecordsSortedByAge();
                break;
            case 4: {
                int id, age;
                std::string name, condition;
                std::cout << "Enter ID: ";
                std::cin >> id;
                std::cin.ignore(); // Clear the newline character from input buffer
                std::cout << "Enter New Name: ";
                std::getline(std::cin, name);
                std::cout << "Enter New Age: ";
                std::cin >> age;
                std::cin.ignore(); // Clear the newline character from input buffer
                std::cout << "Enter New Condition: ";
                std::getline(std::cin, condition);
                updateRecord(id, name, age, condition);
                break;
            }
            case 5: {
                int id;
                std::cout << "Enter ID: ";
                std::cin >> id;
                deleteRecord(id);
                break;
            }
            case 6:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
