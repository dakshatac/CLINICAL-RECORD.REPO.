//Defining Data model
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Patient {
public:
    int id;
    std::string name;
    int age;
    std::string gender;
    std::string diagnosis;
    std::string treatment;

    Patient(int _id, std::string _name, int _age, std::string _gender, std::string _diagnosis, std::string _treatment)
        : id(_id), name(_name), age(_age), gender(_gender), diagnosis(_diagnosis), treatment(_treatment) {}
};
class Database {
private:
    std::vector<Patient> records;
    std::string filename;

    void loadFromFile() {
        std::ifstream file(filename);
        if (file.is_open()) {
            records.clear();
            int id, age;
            std::string name, gender, diagnosis, treatment;
            while (file >> id >> name >> age >> gender >> diagnosis >> treatment) {
                records.emplace_back(id, name, age, gender, diagnosis, treatment);
            }
            file.close();
        }
    }

    void saveToFile() {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (const auto& patient : records) {
                file << patient.id << " " << patient.name << " " << patient.age << " " << patient.gender << " "
                     << patient.diagnosis << " " << patient.treatment << std::endl;
            }
            file.close();
        }
    }

public:
    Database(std::string _filename) : filename(_filename) {
        loadFromFile();
    }

    ~Database() {
        saveToFile();
    }

    void addPatient(const Patient& patient) {
        records.push_back(patient);
        saveToFile();
    }

    void updatePatient(int id, const Patient& newPatient) {
        for (auto& patient : records) {
            if (patient.id == id) {
                patient = newPatient;
                saveToFile();
                return;
            }
        }
    }

    void deletePatient(int id) {
        records.erase(std::remove_if(records.begin(), records.end(), [id](const Patient& patient) {
            return patient.id == id;
        }), records.end());
        saveToFile();
    }

    Patient* searchPatient(int id) {
        for (auto& patient : records) {
            if (patient.id == id) {
                return &patient;
            }
        }
        return nullptr;
    }

    void listPatients() {
        for (const auto& patient : records) {
            std::cout << "ID: " << patient.id << ", Name: " << patient.name << ", Age: " << patient.age
                      << ", Gender: " << patient.gender << ", Diagnosis: " << patient.diagnosis
                      << ", Treatment: " << patient.treatment << std::endl;
        }
    }
};
void showMenu() {
    std::cout << "1. Add Patient\n";
    std::cout << "2. Update Patient\n";
    std::cout << "3. Delete Patient\n";
    std::cout << "4. Search Patient\n";
    std::cout << "5. List Patients\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    Database db("patients.txt");

    while (true) {
        showMenu();
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int id, age;
            std::string name, gender, diagnosis, treatment;
            std::cout << "Enter ID: ";
            std::cin >> id;
            std::cout << "Enter Name: ";
            std::cin >> name;
            std::cout << "Enter Age: ";
            std::cin >> age;
            std::cout << "Enter Gender: ";
            std::cin >> gender;
            std::cout << "Enter Diagnosis: ";
            std::cin >> diagnosis;
            std::cout << "Enter Treatment: ";
            std::cin >> treatment;
            db.addPatient(Patient(id, name, age, gender, diagnosis, treatment));
        } else if (choice == 2) {
            int id, age;
            std::string name, gender, diagnosis, treatment;
            std::cout << "Enter ID of patient to update: ";
            std::cin >> id;
            std::cout << "Enter Name: ";
            std::cin >> name;
            std::cout << "Enter Age: ";
            std::cin >> age;
            std::cout << "Enter Gender: ";
            std::cin >> gender;
            std::cout << "Enter Diagnosis: ";
            std::cin >> diagnosis;
            std::cout << "Enter Treatment: ";
            std::cin >> treatment;
            db.updatePatient(id, Patient(id, name, age, gender, diagnosis, treatment));
        } else if (choice == 3) {
            int id;
            std::cout << "Enter ID of patient to delete: ";
            std::cin >> id;
            db.deletePatient(id);
        } else if (choice == 4) {
            int id;
            std::cout << "Enter ID of patient to search: ";
            std::cin >> id;
            Patient* patient = db.searchPatient(id);
            if (patient) {
                std::cout << "ID: " << patient->id << ", Name: " << patient->name << ", Age: " << patient->age
                          << ", Gender: " << patient->gender << ", Diagnosis: " << patient->diagnosis
                          << ", Treatment: " << patient->treatment << std::endl;
            } else {
                std::cout << "Patient not found.\n";
            }
        } else if (choice == 5) {
            db.listPatients();
        } else if (choice == 6) {
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

