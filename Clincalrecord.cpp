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

    // Constructor for convenience
    PatientRecord(int id, std::string name, int age, std::string condition)
        : id(id), name(name), age(age), condition(condition) {}
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
    // Adding some records
    addRecord(1, "Alice", 30, "Flu");
    addRecord(2, "Bob", 25, "Cold");
    addRecord(3, "Charlie", 35, "Asthma");

    // Retrieve records by ID
    std::cout << "\nRetrieving record with ID 2:\n";
    retrieveRecordByID(2);

    // Retrieve all records sorted by age
    std::cout << "\nRetrieving all records sorted by age:\n";
    retrieveRecordsSortedByAge();

    // Update a record
    std::cout << "\nUpdating record with ID 2:\n";
    updateRecord(2, "Bob", 26, "Recovered from Cold");

    // Retrieve all records after update
    std::cout << "\nRetrieving all records sorted by age after update:\n";
    retrieveRecordsSortedByAge();

    // Delete a record
    std::cout << "\nDeleting record with ID 1:\n";
    deleteRecord(1);

    // Retrieve all records after deletion
    std::cout << "\nRetrieving all records sorted by age after deletion:\n";
    retrieveRecordsSortedByAge();

    return 0;
}
