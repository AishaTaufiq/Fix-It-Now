#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
using namespace std;


class Customer;
class ServiceProvider;
/* === User class === */
class User {
protected:
    int id;
    string name;
    string password;
public:
    User() {}
    User(int id, string name, string password) : id(id), name(name), password(password) {}
    int getId() { return id; }
    string getName() { return name; }
    string getPassword() { return password; }
    virtual void display() = 0;
};

/* === Customer class === */
class Customer : public User {
public:
    Customer() {}
    Customer(int id, string name, string password) : User(id, name, password) {}

    void display() {
        cout << "Customer ID: " << id << ", Name: " << name << endl;
    }
};

/* === Service Provider class === */
class ServiceProvider : public User {
    string expertise;
    float minBudget;
    float maxBudget;
    float totalRating;
    int numberOfRatings;
    vector<string> bookings; 
public:
    ServiceProvider() {
        totalRating = 0;
        numberOfRatings = 0;
    }
    ServiceProvider(int id, string name, string password, string expertise, float minB, float maxB)
        : User(id, name, password), expertise(expertise), minBudget(minB), maxBudget(maxB) {
        totalRating = 0;
        numberOfRatings = 0;
    }

    void display() {
        cout << "Provider ID: " << id << ", Name: " << name << ", Expertise: " << expertise
             << ", Price Range: " << minBudget << "-" << maxBudget 
             << ", Rating: " << getAverageRating() << " stars" << endl;
    }

    void updateRating(float stars) {
        totalRating += stars;
        numberOfRatings++;
    }

    float getAverageRating() {
        if (numberOfRatings == 0)
            return 0;
        return totalRating / numberOfRatings;
    }

    string getExpertise() { return expertise; }
    float getMinBudget() { return minBudget; }
    float getMaxBudget() { return maxBudget; }
    
    void addBooking(string bookingInfo) {
        bookings.push_back(bookingInfo);
    }
    
    void viewBookings() {
        if (bookings.empty()) {
            cout << "No bookings yet." << endl;
            return;
        }
        cout << "\n--- Your Bookings ---" << endl;
        for (size_t i = 0; i < bookings.size(); i++) {
            cout << bookings[i] << endl;
        }
    }
};

/* === Service class === */
class Service {
public:
    string category;
    float estimatedCost;

    Service(string c, float cost) {
        category = c;
        estimatedCost = cost;
    }

    void displayServiceInfo() {
        cout << "Service: " << category << " | Estimated Cost: " << estimatedCost << endl;
    }
};

/* === Booking class === */
class Booking {
public:
    Customer* customer;
    ServiceProvider* provider;
    Service* service;
    string date, time, status;

    Booking(Customer* c, ServiceProvider* p, Service* s, string d, string t) {
        customer = c;
        provider = p;
        service = s;
        date = d;
        time = t;
        status = "Pending";
    }

    void updateStatus(string newStatus) {
        status = newStatus;
    }

    void displayBookingInfo() {
        cout << "\n--- Booking Info ---" << endl;
        customer->display();
        provider->display();
        service->displayServiceInfo();
        cout << "Date: " << date << ", Time: " << time << ", Status: " << status << endl;
    }
    
    string getBookingInfo() {
        return "Customer: " + customer->getName() + " | Service: " + service->category + 
               " | Date: " + date + " | Time: " + time + " | Status: " + status;
    }
};

// ===== CHATBOT SUGGESTION CLASS =====
class Chatbot {
public:
    static vector<ServiceProvider*> suggestWorkers(vector<ServiceProvider>& providers, string neededExpertise, float budget) {
        vector<ServiceProvider*> suitableProviders;
        for (size_t i = 0; i < providers.size(); i++) {
            if (providers[i].getExpertise() == neededExpertise &&
                budget >= providers[i].getMinBudget() &&
                budget <= providers[i].getMaxBudget()) {
                suitableProviders.push_back(&providers[i]);
            }
        }
        return suitableProviders;
    }
};

//                                 ===== FILE HANDLING FUNCTIONS =====
int generateID(const string& filename) {
    ifstream fin(filename.c_str());
    if (!fin) {
        return 1000;
    }

    int lastID = 1000;
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) {
            lastID++;
        }
    }
    fin.close();
    return lastID + 1;
}

void loadProviders(vector<ServiceProvider>& providers) {
    ifstream fin("providers.txt");
    if (!fin) return;

    int id;
    string name, password, expertise;
    float minB, maxB;
    
    while (fin >> id >> name >> password >> expertise >> minB >> maxB) {
        providers.push_back(ServiceProvider(id, name, password, expertise, minB, maxB));
    }
    fin.close();
}

//                                     ===== SIGNUP FUNCTIONS =====
void signupCustomer() {
    string name, password;
    cout << "Enter your name: ";
    cin >> name;
    cout << "Enter your password: ";
    cin >> password;
    int id = generateID("customers.txt");
    
    ofstream fout("customers.txt", ios::app);
    fout << id << " " << name << " " << password << endl;
    fout.close();
    
    cout << "Signup successful! Your ID is: " << id << endl;
}

void signupProvider() {
    string name, password, expertise;
    float minB, maxB;
    cout << "Enter your name: ";
    cin >> name;
    cout << "Enter your password: ";
    cin >> password;
    cout << "Enter your expertise (Plumbing/Painting/Electrical/Carpentry): ";
    cin >> expertise;
    cout << "Enter minimum budget: ";
    cin >> minB;
    cout << "Enter maximum budget: ";
    cin >> maxB;
    
    int id = generateID("providers.txt");
    
    ofstream fout("providers.txt", ios::app);
    fout << id << " " << name << " " << password << " " << expertise << " " << minB << " " << maxB << endl;
    fout.close();
    
    cout << "Provider registration successful! Your ID is: " << id << endl;
}

//                                     ===== LOGIN FUNCTIONS =====
bool loginCustomer(Customer& loggedInCustomer) {
    int id;
    string password;
    cout << "Enter your ID: ";
    cin >> id;
    cout << "Enter your password: ";
    cin >> password;

    ifstream fin("customers.txt");
    if (!fin) {
        cout << "No customers registered yet.\n";
        return false;
    }

    int file_id;
    string file_name, file_pass;
    bool found = false;
    while (fin >> file_id >> file_name >> file_pass) {
        if (file_id == id && file_pass == password) {
            found = true;
            loggedInCustomer = Customer(file_id, file_name, file_pass);
            break;
        }
    }
    fin.close();

    if (found) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Invalid ID or password.\n";
        return false;
    }
}

bool loginProvider(ServiceProvider& loggedInProvider, vector<ServiceProvider>& providers) {
    int id;
    string password;
    cout << "Enter your ID: ";
    cin >> id;
    cout << "Enter your password: ";
    cin >> password;

    for (size_t i = 0; i < providers.size(); i++) {
        if (providers[i].getId() == id && providers[i].getPassword() == password) {
            loggedInProvider = providers[i];
            cout << "Login successful!\n";
            return true;
        }
    }
    cout << "Invalid ID or password.\n";
    return false;
}

//                               ===== SERVICE BOOKING FUNCTION =====
void bookService(Customer& customer, vector<ServiceProvider>& providers, vector<Service>& services) {
    int serviceChoice;
    string neededService;
    cout << "\nAvailable Services: \n";
    cout << "1. Plumbing\n";
    cout << "2. Electrical Repair\n";
    cout << "3. Painting\n";
    cout << "4. Carpentry\n";
    cout << "Enter your choice (1-4): ";
    cin >> serviceChoice;
    
    switch(serviceChoice) {
        case 1:
		 neededService = "Plumbing";
		break;
        case 2: 
		 neededService = "Electrical"; 
		break;
        case 3:
		 neededService = "Painting"; 
		break;
        case 4:
		 neededService = "Carpentry"; 
		break;
        default:
            cout << "Invalid choice.\n";
        return;
    }

    float userBudget;
    cout << "Enter your budget for this service: ";
    cin >> userBudget;
    cin.ignore(); 

    Service* selectedService = NULL;
    for (size_t i = 0; i < services.size(); i++) {
        if (services[i].category == neededService) {
            selectedService = &services[i];
            break;
        }
    }

    if (!selectedService) {
        cout << "Service not available at the moment :(\n";
        return;
    }

    vector<ServiceProvider*> suitableProviders = Chatbot::suggestWorkers(providers, neededService, userBudget);

    if (suitableProviders.empty()) {
        cout << "\nNo service providers found matching your needs and budget.\n";
        cout << "Try adjusting your budget. Current service price range: " << selectedService->estimatedCost << endl;
        return;
    }

    cout << "\nAvailable Service Providers in your budget:\n";
    for (size_t i = 0; i < suitableProviders.size(); i++) {
        cout << i+1 << ". ";
        suitableProviders[i]->display();
    }

    int providerChoice;
    cout << "\nEnter the s.no of the provider you want to book: ";
    cin >> providerChoice;
    cin.ignore(); 

    if (providerChoice < 1 || providerChoice > (int)suitableProviders.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    ServiceProvider* chosenProvider = suitableProviders[providerChoice-1];

    
    string date, time;
    cout << "Enter preferred date (YYYY-MM-DD): ";
    getline(cin, date);
    cout << "Enter preferred time (HH:MM): ";
    getline(cin, time);


    Booking booking(&customer, chosenProvider, selectedService, date, time);
    cout << "\nBooking Created Successfully!" << endl;
    booking.displayBookingInfo();

    string newStatus;
    cout << "\nEnter New Status (Pending/Completed): ";
    getline(cin, newStatus);
    booking.updateStatus(newStatus);
    cout << "\nStatus Updated Successfully!\n";


    chosenProvider->addBooking(booking.getBookingInfo());

    char rateChoice;
    cout << "\nWould you like to rate your service provider? (y/n): ";
    cin >> rateChoice;
    if (rateChoice == 'y' || rateChoice == 'Y') {
        float stars;
        cout << "Give Rating (1-5 stars): ";
        cin >> stars;
        while (stars < 1 || stars > 5) {
            cout << "you can only rate max 5 stars at the moment\nPlease enter a rating between 1 and 5: ";
            cin >> stars;
        }
        chosenProvider->updateRating(stars);
        cout << "Thank you! New Average Rating for " << chosenProvider->getName() 
             << ": " << chosenProvider->getAverageRating() << " stars" << endl;
    } else {
        cout << "No rating given.\n";
    }
}

//                                        ===== MAIN MENU =====
void mainMenu() {
    vector<ServiceProvider> providers;
    vector<Service> services;
    
    services.push_back(Service("Plumbing", 550));
    services.push_back(Service("Electrical", 750));
    services.push_back(Service("Painting", 450));
    services.push_back(Service("Carpentry", 650));
    
    loadProviders(providers);

    int choice;
    Customer loggedInCustomer;
    ServiceProvider loggedInProvider;
    
    while (true) {
        cout << "\n ----- FIXITNOW MAIN MENU -----" << endl;
        cout << " The best home service provider " << endl;
        cout << "  ____________________________  " << endl;
        cout << "|1. Signup as Customer         |" << endl;
        cout << "|2. Signup as Service Provider |"<< endl;
        cout << "|3. Login as Customer          |"<< endl;
        cout << "|4. Login as Service Provider  |"<< endl;
        cout << "|5. Exit                       |"<< endl;
        cout << "| ____________________________ |" << endl;
        cout << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            signupCustomer();
        } 
        else if (choice == 2) {
            signupProvider();
            providers.clear();
            loadProviders(providers);
        } 
        else if (choice == 3) {
            if (loginCustomer(loggedInCustomer)) {
            	
                cout << "Welcome to FixItNow :) ," << loggedInCustomer.getName() << "! (ID: " << loggedInCustomer.getId() << ")\n";
                
                int serviceChoice;
                do {
                	cout << "_______________________________";
                    cout << "\n----- SERVICE MENU -----\n";
                    cout << "1. Book a Service\n";
                    cout << "2. Logout\n";
                    cout << "Enter choice: ";
                    cin >> serviceChoice;
                    
                    if (serviceChoice == 1) {
                        bookService(loggedInCustomer, providers, services);
                    } 
                    else if (serviceChoice != 2) {
                        cout << "Invalid choice. Try again.\n";
                    }
                } while (serviceChoice != 2);
            }
        }
        else if (choice == 4) {
            if (loginProvider(loggedInProvider, providers)) {
                cout << "Welcome to FixItNow, " << loggedInProvider.getName() << "! (ID: " << loggedInProvider.getId() << ")\n";
                
                int providerChoice;
                do {
                	cout << "_______________________________";
                    cout << "\n----- PROVIDER MENU -----\n";
                    cout << "1. View Your Bookings\n";
                    cout << "2. Logout\n";
                    cout << "Enter choice: ";
                    cin >> providerChoice;
                    
                    if (providerChoice == 1) {
                        loggedInProvider.viewBookings();
                    } 
                    else if (providerChoice != 2) {
                        cout << "Invalid choice. Try again.\n";
                    }
                } while (providerChoice != 2);
            }
        }
        else if (choice == 5) {
            cout << "Goodbye..Have a great day ahead..Happy Helping :)\n";
            break;
        } 
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }
}

int main() {
    mainMenu();
    return 0;
}
