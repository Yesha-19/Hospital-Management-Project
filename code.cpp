#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <limits>
#include <cctype>
using namespace std;

// Utility function to safely convert string to int
int safe_stoi(const string &str, int default_value = 0)
{
    if (str.empty())
        return default_value;
    try
    {
        return stoi(str);
    }
    catch (const invalid_argument &e)
    {
        return default_value;
    }
    catch (const out_of_range &e)
    {
        return default_value;
    }
}

// Utility function to safely convert string to double
double safe_stod(const string &str, double default_value = 0.0)
{
    if (str.empty())
        return default_value;
    try
    {
        return stod(str);
    }
    catch (const invalid_argument &e)
    {
        return default_value;
    }
    catch (const out_of_range &e)
    {
        return default_value;
    }
}

// Abstract base class Person (Data Abstraction, Virtual Functions)
class Person
{
protected:
    string name;

public:
    Person(string n) : name(n) {}
    virtual ~Person() {}
    virtual void display() const = 0;
    virtual void save(ofstream &out) const = 0;
    virtual void load(ifstream &in) = 0;
    string getName() const { return name; }
    void setName(string n) { name = n; }
};

// Doctor class inheriting from Person
class Doctor : public Person
{
private:
    vector<string> specialties;
    int patientCount;
    double surcharge;

public:
    Doctor(string n, vector<string> spec, double sur) : Person(n), specialties(spec), patientCount(0), surcharge(sur) {}
    Doctor() : Person(""), patientCount(0), surcharge(0) {}
    ~Doctor() {}

    void display() const override
    {
        cout << "Doctor: " << name << ", Specialties: ";
        for (size_t i = 0; i < specialties.size(); i++)
        {
            cout << specialties[i];
            if (i < specialties.size() - 1)
                cout << ", ";
        }
        cout << ", Patients: " << patientCount << ", Surcharge: Rs." << surcharge << "\n";
    }

    void save(ofstream &out) const override
    {
        out << "DOCTOR\n";
        out << name << "\n";
        out << patientCount << "\n";
        out << surcharge << "\n";
        out << specialties.size() << "\n";
        for (auto &s : specialties)
            out << s << "\n";
    }

    void load(ifstream &in) override
    {
        string marker;
        getline(in, marker); // Read DOCTOR marker

        getline(in, name);

        string temp;
        getline(in, temp);
        patientCount = safe_stoi(temp, 0);

        getline(in, temp);
        surcharge = safe_stod(temp, 0.0);

        getline(in, temp);
        int numSpec = safe_stoi(temp, 0);

        specialties.clear();
        for (int j = 0; j < numSpec; j++)
        {
            string spec;
            if (getline(in, spec))
            {
                if (!spec.empty())
                {
                    specialties.push_back(spec);
                }
            }
        }
    }

    vector<string> getSpecialties() const { return specialties; }
    int getPatientCount() const { return patientCount; }
    void setPatientCount(int c) { patientCount = c; }
    double getSurcharge() const { return surcharge; }
};

// Patient class inheriting from Person
class Patient : public Person
{
protected:
    string disease;
    string assignedDoctor;
    string severity;
    int roomNumber;

public:
    Patient(string n, string d, string doc, string sev, int room) : Person(n), disease(d), assignedDoctor(doc), severity(sev), roomNumber(room) {}
    Patient() : Person(""), disease(""), assignedDoctor(""), severity(""), roomNumber(0) {}
    virtual ~Patient() {}

    virtual void display() const override
    {
        cout << "Patient: " << name << ", Disease: " << disease << ", Doctor: " << assignedDoctor << ", Severity: " << severity << ", Room: " << roomNumber << "\n";
    }

    virtual void save(ofstream &out) const override
    {
        out << "PATIENT\n";
        out << name << "\n";
        out << disease << "\n";
        out << assignedDoctor << "\n";
        out << severity << "\n";
        out << roomNumber << "\n";
    }

    virtual void load(ifstream &in) override
    {
        string marker;
        getline(in, marker); // Read PATIENT marker

        getline(in, name);
        getline(in, disease);
        getline(in, assignedDoctor);
        getline(in, severity);

        string temp;
        getline(in, temp);
        roomNumber = safe_stoi(temp, 0);
    }

    virtual double calculateBill(const map<string, double> &diseaseCost, const map<string, double> &severityMultiplier, double surcharge) const
    {
        auto diseaseIt = diseaseCost.find(disease);
        auto severityIt = severityMultiplier.find(severity);

        if (diseaseIt == diseaseCost.end() || severityIt == severityMultiplier.end())
        {
            return 500.0; // Default cost
        }

        return diseaseIt->second * severityIt->second + surcharge;
    }

    string getDisease() const { return disease; }
    string getAssignedDoctor() const { return assignedDoctor; }
    string getSeverity() const { return severity; }
    int getRoomNumber() const { return roomNumber; }
    void setRoomNumber(int r) { roomNumber = r; }
};

// EmergencyPatient inheriting from Patient
class EmergencyPatient : public Patient
{
public:
    EmergencyPatient(string n, string d, string doc, string sev, int room) : Patient(n, d, doc, sev, room) {}
    EmergencyPatient() : Patient() {}
    ~EmergencyPatient() {}

    void display() const override
    {
        cout << "Emergency Patient: " << name << ", Disease: " << disease << ", Doctor: " << assignedDoctor << ", Severity: " << severity << ", Room: " << roomNumber << "\n";
    }

    void save(ofstream &out) const override
    {
        out << "EMERGENCY\n";
        out << name << "\n";
        out << disease << "\n";
        out << assignedDoctor << "\n";
        out << severity << "\n";
        out << roomNumber << "\n";
    }

    void load(ifstream &in) override
    {
        string marker;
        getline(in, marker); // Read EMERGENCY marker

        getline(in, name);
        getline(in, disease);
        getline(in, assignedDoctor);
        getline(in, severity);

        string temp;
        getline(in, temp);
        roomNumber = safe_stoi(temp, 0);
    }

    double calculateBill(const map<string, double> &diseaseCost, const map<string, double> &severityMultiplier, double surcharge) const override
    {
        double base = Patient::calculateBill(diseaseCost, severityMultiplier, surcharge);
        return base * 1.5;
    }
};

// Global room management
const int TOTAL_ROOMS = 100;
vector<bool> rooms(TOTAL_ROOMS, false);

int findAvailableRoom()
{
    for (int i = 0; i < (int)rooms.size(); ++i)
    {
        if (!rooms[i])
            return i;
    }
    return -1;
}

// Hospital class
class Hospital
{
private:
    vector<Doctor *> doctors;
    vector<Patient *> patients;
    map<string, double> diseaseCost;
    map<string, double> severityMultiplier;
    const string dataFile = "hospital_data.txt";

    void initializeDoctors()
    {
        // Clear existing doctors
        for (auto d : doctors)
            delete d;
        doctors.clear();

        // Initialize with fresh doctors
        doctors.push_back(new Doctor("Dr. Smith", {"Flu", "Cold"}, 800));
        doctors.push_back(new Doctor("Dr. Jones", {"Diabetes", "Hypertension"}, 1500));
        doctors.push_back(new Doctor("Dr. Brown", {"Asthma", "Allergy"}, 1200));
        doctors.push_back(new Doctor("Dr. Taylor", {"Fever", "Flu"}, 900));
        doctors.push_back(new Doctor("Dr. Wilson", {"Cold", "Migraine"}, 700));
        doctors.push_back(new Doctor("Dr. Moore", {"Diabetes", "Obesity"}, 2000));
        doctors.push_back(new Doctor("Dr. Clark", {"Hypertension", "Heart Disease"}, 2500));
        doctors.push_back(new Doctor("Dr. Lewis", {"Allergy", "Skin Infection"}, 800));
        doctors.push_back(new Doctor("Dr. Hall", {"Asthma", "Pneumonia"}, 1800));
        doctors.push_back(new Doctor("Dr. Allen", {"Fever", "Infection"}, 1000));
    }

public:
    Hospital()
    {
        diseaseCost = {
            {"Flu", 1000}, {"Cold", 500}, {"Fever", 800}, {"Diabetes", 4000}, {"Hypertension", 3000}, {"Asthma", 2500}, {"Allergy", 1200}, {"Migraine", 1500}, {"Obesity", 3500}, {"Heart Disease", 5000}, {"Skin Infection", 1000}, {"Pneumonia", 4500}, {"Infection", 2000}};

        severityMultiplier = {{"Mild", 1.0}, {"Moderate", 1.5}, {"Severe", 2.0}};

        initializeDoctors(); // Always start with fresh doctors
        loadFromFile();
    }

    ~Hospital()
    {
        for (auto d : doctors)
            delete d;
        for (auto p : patients)
            delete p;
    }

    void showDiseases()
    {
        cout << "Available diseases:\n";
        vector<string> diseaseList;
        for (auto doc : doctors)
        {
            for (auto &d : doc->getSpecialties())
            {
                if (find(diseaseList.begin(), diseaseList.end(), d) == diseaseList.end())
                {
                    diseaseList.push_back(d);
                }
            }
        }

        for (size_t i = 0; i < diseaseList.size(); i++)
        {
            cout << i + 1 << ". " << diseaseList[i] << "\n";
        }
    }

    void showDoctorsForDisease(const string &disease)
    {
        cout << "Doctors who can treat " << disease << ":\n";
        bool found = false;
        for (auto doc : doctors)
        {
            auto specs = doc->getSpecialties();
            if (find(specs.begin(), specs.end(), disease) != specs.end())
            {
                cout << "- " << doc->getName() << " (Current Patients: " << doc->getPatientCount() << ", Surcharge: Rs." << doc->getSurcharge() << ")\n";
                found = true;
            }
        }
        if (!found)
        {
            cout << "No doctors found for this disease.\n";
        }
    }

    double getDoctorSurcharge(const string &doctorName)
    {
        for (auto d : doctors)
            if (d->getName() == doctorName)
                return d->getSurcharge();
        return 0;
    }

    string recommendLeastCostDoctor(const string &disease, const string &severity)
    {
        double minCost = 1e9;
        string recommended;
        for (auto doc : doctors)
        {
            auto specs = doc->getSpecialties();
            if (find(specs.begin(), specs.end(), disease) != specs.end())
            {
                if (diseaseCost.find(disease) != diseaseCost.end() &&
                    severityMultiplier.find(severity) != severityMultiplier.end())
                {
                    double cost = diseaseCost.at(disease) * severityMultiplier.at(severity) + doc->getSurcharge();
                    if (cost < minCost)
                    {
                        minCost = cost;
                        recommended = doc->getName();
                    }
                }
            }
        }
        return recommended.empty() ? doctors[0]->getName() : recommended;
    }

    void addPatient()
    {
        string name, disease, severity, assignedDoctor;
        cout << "Enter patient name: ";
        cin.ignore();
        getline(cin, name);

        if (name.empty())
        {
            cout << "Invalid name!\n";
            return;
        }

        showDiseases();
        int choice;
        cout << "Choose disease number: ";
        cin >> choice;

        vector<string> diseaseList;
        for (auto doc : doctors)
        {
            for (auto &d : doc->getSpecialties())
            {
                if (find(diseaseList.begin(), diseaseList.end(), d) == diseaseList.end())
                    diseaseList.push_back(d);
            }
        }

        if (choice < 1 || choice > (int)diseaseList.size())
        {
            cout << "Invalid choice!\n";
            return;
        }
        disease = diseaseList[choice - 1];

        cout << "Enter severity (Mild/Moderate/Severe): ";
        cin >> severity;

        if (severity != "Mild" && severity != "Moderate" && severity != "Severe")
        {
            cout << "Invalid severity! Using 'Mild' as default.\n";
            severity = "Mild";
        }

        showDoctorsForDisease(disease);
        string recommended = recommendLeastCostDoctor(disease, severity);
        cout << "\nRecommended doctor (least cost): " << recommended << "\n";
        cout << "Do you want to accept this doctor? (y/n): ";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
        {
            assignedDoctor = recommended;
        }
        else
        {
            cout << "Enter the name of doctor you want: ";
            cin.ignore();
            getline(cin, assignedDoctor);

            bool doctorExists = false;
            for (auto doc : doctors)
            {
                if (doc->getName() == assignedDoctor)
                {
                    doctorExists = true;
                    break;
                }
            }
            if (!doctorExists)
            {
                cout << "Doctor not found! Using recommended doctor.\n";
                assignedDoctor = recommended;
            }
        }

        int roomIndex = findAvailableRoom();
        if (roomIndex == -1)
        {
            cout << " Sorry, no rooms are currently available. Cannot admit patient.\n";
            return;
        }

        for (auto doc : doctors)
        {
            if (doc->getName() == assignedDoctor)
            {
                doc->setPatientCount(doc->getPatientCount() + 1);
                break;
            }
        }

        Patient *p = new Patient(name, disease, assignedDoctor, severity, roomIndex + 1);
        rooms[roomIndex] = true;
        patients.push_back(p);
        cout << "Patient added! Assigned Doctor: " << assignedDoctor << ", Room: " << p->getRoomNumber() << "\n";
    }

    void addEmergencyPatient()
    {
        string name, disease, severity, assignedDoctor;
        cout << "Enter emergency patient name: ";
        cin.ignore();
        getline(cin, name);

        if (name.empty())
        {
            cout << "Invalid name!\n";
            return;
        }

        showDiseases();
        int choice;
        cout << "Choose disease number: ";
        cin >> choice;

        vector<string> diseaseList;
        for (auto doc : doctors)
        {
            for (auto &d : doc->getSpecialties())
            {
                if (find(diseaseList.begin(), diseaseList.end(), d) == diseaseList.end())
                    diseaseList.push_back(d);
            }
        }

        if (choice < 1 || choice > (int)diseaseList.size())
        {
            cout << "Invalid choice!\n";
            return;
        }
        disease = diseaseList[choice - 1];

        cout << "Enter severity (Mild/Moderate/Severe): ";
        cin >> severity;

        if (severity != "Mild" && severity != "Moderate" && severity != "Severe")
        {
            cout << "Invalid severity! Using 'Moderate' as default.\n";
            severity = "Moderate";
        }

        string recommended = recommendLeastCostDoctor(disease, severity);
        cout << "\nRecommended doctor (least cost): " << recommended << "\n";
        assignedDoctor = recommended;

        int roomIndex = findAvailableRoom();
        if (roomIndex == -1)
        {
            cout << " Sorry, no rooms are currently available for this emergency patient.\n";
            return;
        }

        for (auto doc : doctors)
        {
            if (doc->getName() == assignedDoctor)
            {
                doc->setPatientCount(doc->getPatientCount() + 1);
                break;
            }
        }

        EmergencyPatient *ep = new EmergencyPatient(name, disease, assignedDoctor, severity, roomIndex + 1);
        rooms[roomIndex] = true;
        patients.push_back(ep);
        cout << "Emergency patient added! Assigned Doctor: " << assignedDoctor << ", Room: " << ep->getRoomNumber() << "\n";
    }

    void showAllPatients()
    {
        if (patients.empty())
        {
            cout << "No patients in the system.\n";
            return;
        }

        cout << "Patients List:\n";
        for (size_t i = 0; i < patients.size(); i++)
        {
            cout << i + 1 << ". ";
            patients[i]->display();
        }
    }

    void showAllEmergencyPatients()
    {
        bool found = false;
        cout << "Emergency Patients List:\n";
        for (size_t i = 0; i < patients.size(); i++)
        {
            if (dynamic_cast<EmergencyPatient *>(patients[i]))
            {
                cout << i + 1 << ". ";
                patients[i]->display();
                found = true;
            }
        }

        if (!found)
            cout << "No emergency patients in the system.\n";
    }

    void showAllDoctors()
    {
        cout << "Doctors List:\n";
        for (auto d : doctors)
            d->display();
    }

    void generateBill()
    {
        if (patients.empty())
        {
            cout << "No patients in system.\n";
            return;
        }
        cout << "Select patient number for bill:\n";
        showAllPatients();
        int choice;
        cin >> choice;
        if (choice < 1 || choice > (int)patients.size())
        {
            cout << "Invalid choice!\n";
            return;
        }

        Patient *p = patients[choice - 1];
        double surcharge = getDoctorSurcharge(p->getAssignedDoctor());
        double cost = p->calculateBill(diseaseCost, severityMultiplier, surcharge);

        cout << "\n===== BILL =====\n";
        cout << "Patient Name: " << p->getName() << "\n";
        cout << "Disease: " << p->getDisease() << "\n";
        cout << "Severity: " << p->getSeverity() << "\n";
        cout << "Assigned Doctor: " << p->getAssignedDoctor() << "\n";
        cout << "Room Number: " << p->getRoomNumber() << "\n";
        cout << "Total Cost: ₹" << cost << "\n";
        cout << "================\n";
    }

    void generateEmergencyBill()
    {
        vector<int> emergencyIndices;
        for (size_t i = 0; i < patients.size(); i++)
        {
            if (dynamic_cast<EmergencyPatient *>(patients[i]))
            {
                emergencyIndices.push_back(i);
            }
        }

        if (emergencyIndices.empty())
        {
            cout << "No emergency patients in system.\n";
            return;
        }

        cout << "Select emergency patient number for bill:\n";
        for (size_t i = 0; i < emergencyIndices.size(); i++)
        {
            cout << i + 1 << ". ";
            patients[emergencyIndices[i]]->display();
        }

        int choice;
        cin >> choice;
        if (choice < 1 || choice > (int)emergencyIndices.size())
        {
            cout << "Invalid choice!\n";
            return;
        }

        Patient *ep = patients[emergencyIndices[choice - 1]];
        double surcharge = getDoctorSurcharge(ep->getAssignedDoctor());
        double cost = ep->calculateBill(diseaseCost, severityMultiplier, surcharge);

        cout << "\n===== EMERGENCY BILL =====\n";
        cout << "Patient Name: " << ep->getName() << "\n";
        cout << "Disease: " << ep->getDisease() << "\n";
        cout << "Severity: " << ep->getSeverity() << "\n";
        cout << "Assigned Doctor: " << ep->getAssignedDoctor() << "\n";
        cout << "Room Number: " << ep->getRoomNumber() << "\n";
        cout << "Total Emergency Cost: Rs." << cost << "\n";
        cout << "==========================\n";
    }

    void dischargePatient()
    {
        if (patients.empty())
        {
            cout << "No patients to discharge.\n";
            return;
        }
        cout << "Select patient number to discharge:\n";
        showAllPatients();
        int choice;
        cin >> choice;
        if (choice < 1 || choice > (int)patients.size())
        {
            cout << "Invalid choice!\n";
            return;
        }

        Patient *p = patients[choice - 1];
        string docName = p->getAssignedDoctor();
        for (auto doc : doctors)
        {
            if (doc->getName() == docName)
            {
                doc->setPatientCount(doc->getPatientCount() - 1);
                break;
            }
        }

        int roomIndex = p->getRoomNumber() - 1;
        if (roomIndex >= 0 && roomIndex < (int)rooms.size())
        {
            rooms[roomIndex] = false;
        }

        cout << "Patient " << p->getName() << " discharged and room " << p->getRoomNumber() << " is now free.\n";
        delete p;
        patients.erase(patients.begin() + choice - 1);
    }

    void dischargeEmergencyPatient()
    {
        vector<int> emergencyIndices;
        for (size_t i = 0; i < patients.size(); i++)
        {
            if (dynamic_cast<EmergencyPatient *>(patients[i]))
            {
                emergencyIndices.push_back(i);
            }
        }

        if (emergencyIndices.empty())
        {
            cout << "No emergency patients to discharge.\n";
            return;
        }

        cout << "Select emergency patient number to discharge:\n";
        for (size_t i = 0; i < emergencyIndices.size(); i++)
        {
            cout << i + 1 << ". ";
            patients[emergencyIndices[i]]->display();
        }

        int choice;
        cin >> choice;
        if (choice < 1 || choice > (int)emergencyIndices.size())
        {
            cout << "Invalid choice!\n";
            return;
        }

        int actualIndex = emergencyIndices[choice - 1];
        Patient *ep = patients[actualIndex];
        string docName = ep->getAssignedDoctor();
        for (auto doc : doctors)
        {
            if (doc->getName() == docName)
            {
                doc->setPatientCount(doc->getPatientCount() - 1);
                break;
            }
        }

        int roomIndex = ep->getRoomNumber() - 1;
        if (roomIndex >= 0 && roomIndex < (int)rooms.size())
        {
            rooms[roomIndex] = false;
        }

        cout << "Emergency Patient " << ep->getName() << " discharged and room " << ep->getRoomNumber() << " is now free.\n";
        delete ep;
        patients.erase(patients.begin() + actualIndex);
    }

    void saveToFile()
    {
        ofstream out(dataFile);
        if (!out)
        {
            cout << "Error saving file.\n";
            return;
        }

        // Save doctors
        out << "DOCTORS " << doctors.size() << "\n";
        for (auto d : doctors)
            d->save(out);

        // Save patients
        out << "PATIENTS " << patients.size() << "\n";
        for (auto p : patients)
            p->save(out);

        // Save rooms
        out << "ROOMS " << rooms.size() << "\n";
        for (bool room : rooms)
            out << (room ? "1" : "0") << "\n";

        out.close();
        cout << "Data saved successfully.\n";
    }

    void loadFromFile()
    {
        ifstream in(dataFile);
        if (!in)
        {
            cout << "No previous data found. Starting fresh.\n";
            return;
        }

        // Clear existing patients but keep fresh doctors
        for (auto p : patients)
            delete p;
        patients.clear();

        string line;
        while (getline(in, line))
        {
            if (line.find("DOCTORS") == 0)
            {
                // Skip doctors section - we use fresh doctors
                int numDoctors = safe_stoi(line.substr(8), 0);
                for (int i = 0; i < numDoctors; i++)
                {
                    string marker;
                    getline(in, marker); // Skip doctor data
                    if (marker != "DOCTOR")
                        break;
                    for (int j = 0; j < 4; j++)
                        getline(in, marker); // Skip doctor details
                    int numSpec = safe_stoi(marker, 0);
                    for (int j = 0; j < numSpec; j++)
                        getline(in, marker); // Skip specialties
                }
            }
            else if (line.find("PATIENTS") == 0)
            {
                int numPatients = safe_stoi(line.substr(9), 0);
                for (int i = 0; i < numPatients; i++)
                {
                    string type;
                    if (!getline(in, type))
                        break;

                    Patient *p;
                    if (type == "EMERGENCY")
                    {
                        p = new EmergencyPatient();
                    }
                    else if (type == "PATIENT")
                    {
                        p = new Patient();
                    }
                    else
                    {
                        // Skip invalid data
                        for (int j = 0; j < 5; j++)
                            getline(in, type);
                        continue;
                    }
                    p->load(in);
                    patients.push_back(p);

                    // Update room status and doctor patient count
                    int roomIndex = p->getRoomNumber() - 1;
                    if (roomIndex >= 0 && roomIndex < (int)rooms.size())
                    {
                        rooms[roomIndex] = true;
                    }

                    string docName = p->getAssignedDoctor();
                    for (auto doc : doctors)
                    {
                        if (doc->getName() == docName)
                        {
                            doc->setPatientCount(doc->getPatientCount() + 1);
                            break;
                        }
                    }
                }
            }
            else if (line.find("ROOMS") == 0)
            {
                int numRooms = safe_stoi(line.substr(6), TOTAL_ROOMS);
                rooms.resize(numRooms, false);
                for (int i = 0; i < numRooms; ++i)
                {
                    string occupied;
                    if (!getline(in, occupied))
                        break;
                    rooms[i] = (occupied == "1");
                }
            }
        }

        in.close();
        cout << "Data loaded successfully.\n";
    }

    void summaryReport()
    {
        cout << "\n===== HOSPITAL SUMMARY REPORT =====\n";
        map<string, int> docPatientCount;
        map<string, double> docRevenue;
        double totalRevenue = 0;

        for (auto doc : doctors)
        {
            docPatientCount[doc->getName()] = 0;
            docRevenue[doc->getName()] = 0;
        }

        for (auto p : patients)
        {
            double surcharge = getDoctorSurcharge(p->getAssignedDoctor());
            double cost = p->calculateBill(diseaseCost, severityMultiplier, surcharge);
            docPatientCount[p->getAssignedDoctor()]++;
            docRevenue[p->getAssignedDoctor()] += cost;
            totalRevenue += cost;
        }

        for (auto doc : doctors)
        {
            cout << "Doctor: " << doc->getName() << ", Patients Treated: " << docPatientCount[doc->getName()]
                 << ", Revenue: Rs." << docRevenue[doc->getName()] << "\n";
        }

        cout << "Total Hospital Revenue: ₹" << totalRevenue << "\n";
        cout << "===================================\n";
    }
};

int main()
{
    Hospital h;
    int choice;
    do
    {
        cout << "\n--- Hospital Management System ---\n";
        cout << "1. Add Patient\n";
        cout << "2. Show All Patients\n";
        cout << "3. Show All Doctors\n";
        cout << "4. Generate Bill\n";
        cout << "5. Discharge Patient\n";
        cout << "6. Save To File\n";
        cout << "7. Summary Report\n";
        cout << "8. Add Emergency Patient\n";
        cout << "9. Show Emergency Patients\n";
        cout << "10. Generate Emergency Bill\n";
        cout << "11. Discharge Emergency Patient\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            h.addPatient();
            break;
        case 2:
            h.showAllPatients();
            break;
        case 3:
            h.showAllDoctors();
            break;
        case 4:
            h.generateBill();
            break;
        case 5:
            h.dischargePatient();
            break;
        case 6:
            h.saveToFile();
            break;
        case 7:
            h.summaryReport();
            break;
        case 8:
            h.addEmergencyPatient();
            break;
        case 9:
            h.showAllEmergencyPatients();
            break;
        case 10:
            h.generateEmergencyBill();
            break;
        case 11:
            h.dischargeEmergencyPatient();
            break;
        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    return 0;
}