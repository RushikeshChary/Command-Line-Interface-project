#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cstdio>

using namespace std;

#define AVG_RECORD 60
// Discount given to Employees.
#define DISCOUNT 0.15 //(i.e., 15 % = (15/100))
// Create three databases - one for CAR database, one for COSTOMER database, and one for EMPLOYEE database.
//  Start with atleast 5 cars, 5 customers, 5 employees and 1 manager

class User
{

public:
    string Name;
    string ID;
    string password;
    void viewAvailableCars();
    // virtual void browseMyCars() = 0;
    // virtual void rentCar() = 0;
    // virtual void returnCar() = 0;
};

void User::viewAvailableCars()
{
    cout << "Here are the available cars: " << endl;
    ifstream cardb;
    string line;
    cardb.open("CARdb.txt");
    // cin.ignore();
    if (cardb)
    {
        while (getline(cardb, line))
        {
            std::stringstream data(line);
            char model;
            int condition;
            string availability;
            int price;
            string dueDate;
            data >> model >> condition >> availability >> price >> dueDate;
            if (availability == "Yes")
                cout << "model: " << model << "\tprice(per day): " << price << endl;
        }
        cardb.close();
    }
    else
    {
        cout << "Error in opening car database" << endl;
        cerr << "Error in opening car database" << endl;
        exit(1);
    }
}

class Car
{
private:
    char Model;
    int Condition;       //(0 - 10)
    string Availability; // Yes or No
    int Price;
    int DueDate;
    string rentedTo;

public:
    Car(char Model, int Condtion, string availability, int price, int dueDate, string rentedTo)
    {
        this->Model = Model;
        this->Condition = Condtion;
        this->Availability = availability;
        this->Price = price;
        this->DueDate = dueDate;
        this->rentedTo = rentedTo;
    }
    void update_database();
};

void Car::update_database()
{
    // cout<<
    ofstream NewFile;
    NewFile.open("car.txt");
    ifstream car_db;
    car_db.open("CARdb.txt");
    string line;
    while (getline(car_db, line))
    {
        stringstream data(line);
        char model;
        int condition;
        string availability;
        int price;
        int dueDate;
        string rentedTo;
        // cout << "here in car database update" << endl;
        data >> model >> condition >> availability >> price >> dueDate >> rentedTo;
        if (this->Model != model)
            NewFile << model << " " << condition << " " << availability << " " << price << " " << dueDate << " " << rentedTo << endl;
        else
        {
            NewFile << this->Model << " " << this->Condition << " " << this->Availability << " " << this->Price << " " << this->DueDate << " " << this->rentedTo << endl;
            // cout << "updated" << endl;
            // cout << this->Model << " " << this->Condition << " " << this->Availability << " " << this->Price << " " << this->DueDate << endl;
        }
    }
    car_db.close();
    NewFile.close();

    if (std::remove("CARdb.txt") != 0)
    {
        // cerr << "Error deleting" << endl;
        cout << "Error deleting" << endl;
    }
    if (std::rename("car.txt", "CARdb.txt") != 0)
    {
        // cerr << "Error in renaming" << endl;
        cout << "Error in renaming" << endl;
    }
    cout << "Car database updated successfully!!" << endl;
    // remove("CARdb.txt");
    // rename("Temp.txt", "CARdb.txt");
}

int nextID(ifstream &db)
{
    // returns unique ID number for Customer and Employee (based on the database passed).
    string line;
    int num = 0;
    while (getline(db, line))
    {

        stringstream data(line);
        string id;
        data >> id;
        int result;
        id = id.substr(0, id.size() - 1);
        stringstream ss(id);
        ss >> result;
        num = max(num, result);
    }
    return num + 1;
}

class Customer : public User
{
private:
    int customer_record;
    int fine;
    string rentedCars;

public:
    static int number_of_customers;
    static int average_customer_record;
    Customer(string Name, string ID, string password, string rentedCars, int fine, int customer_record)
    {
        this->Name = Name;
        this->ID = ID;
        this->password = password;
        this->customer_record = customer_record;
        this->fine = fine;
        this->rentedCars = rentedCars;
        // number_of_customers++;
    }

    void customerSignup();

    int numberOfRentedCars()
    {
        if (rentedCars == "NONE")
            return 0;
        else
            return this->rentedCars.size();
    }
    void addRentedCars(char car)
    {
        if (this->rentedCars == "NONE")
            this->rentedCars = car;
        // Add the rented cars to already rented cars of the customer.
        else
            this->rentedCars += car;
    }

    void removeRentedCars(char car)
    {
        // Remove the rented cars from already rented cars of the customer.
        string carList = this->rentedCars;
        carList.erase(std::remove_if(carList.begin(), carList.end(), [car](char c)
                                     { return c == car; }),
                      carList.end());
        this->rentedCars = carList;
    }
    void updateCustomerRecord(int damage, int excessdaysUsed)
    {
        // Each time, some customer's record is changed, average customer record will be updated.
        int total_customer_record = (number_of_customers * average_customer_record - this->customer_record);
        // Making sure that customer record is non-negative.
        if (excessdaysUsed > 0)
            this->customer_record = max(this->customer_record - (damage * 10) - (excessdaysUsed * 2), 0);
        else
            this->customer_record = max(this->customer_record - (damage * 10), 0);
        average_customer_record = (total_customer_record + this->customer_record) / (number_of_customers);
    }

    int getCustomerRecord()
    {
        return this->customer_record;
    }

    void updateFine(int fine)
    {
        this->fine = fine;
    }

    void update_database();

    // void viewAvailableCars(); -------> already present in userclass.

    void browseMyCars();

    void rentCar();

    void returnCar();

    void Interface();
};

// Declaring the initial average customer record and number of customers.
int Customer::average_customer_record = AVG_RECORD;
int Customer::number_of_customers = 0;

void Customer::customerSignup()
{
    // First increment the number of customers.
    number_of_customers++;
    // Then assign the customer record to average customer record.
    // this->customer_record = this->average_customer_record;
    string database = "CUSTOMERdb.txt";
    fstream fdb;
    fdb.open(database, ios::app);
    if (fdb)
    {
        fdb << this->ID << " " << this->Name << " " << this->password << " "
            << "NONE"
            << " "
            << "0"
            << " " << this->customer_record << endl;
        fdb.close();
        cout << "Registerd successfully!!" << endl;
        cout << "Your ID is " << ID << endl;
        // return C;
    }
    else
    {
        cerr << "Error in opening Customer Database" << endl;
        cout << "Error in opening Customer Database" << endl;
        exit(1);
    }
}

void Customer::Interface()
{
loop:
    cout << "What would you like to do?" << endl;
    cout << "1. View available cars" << endl;
    cout << "2. Browse your current rented cars" << endl;
    cout << "3. Rent a car" << endl;
    cout << "4. Return a car" << endl;
    cout << "5. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        viewAvailableCars();
        break;
    }
    case 2:
    {
        // Browse the customer's cars.
        browseMyCars();
        break;
    }
    case 3:
    {
        // rent a car.
        rentCar();
        break;
    }
    case 4:
    {
        // return a car.
        returnCar();
        break;
    }
    case 5:
    {
        // Go back.
        return;
    }
    default:
    {
        cout << "Invalid choice" << endl;
        break;
    }
    }
    goto loop;
}

void Customer::update_database()
{
    if (this->rentedCars == "")
    {
        this->rentedCars = "NONE";
    }
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("CUSTOMERdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string ID, Name, password, rentedCars;
        int fine, customer_record;
        data >> ID >> Name >> password >> rentedCars >> fine >> customer_record;
        if (this->ID != ID)
            newFile << ID << " " << Name << " " << password << " " << rentedCars << " " << fine << " " << customer_record << endl;
        else
        {
            newFile << this->ID << " " << this->Name << " " << this->password << " " << this->rentedCars << " " << this->fine << " " << this->customer_record << endl;
        }
    }
    db.close();
    newFile.close();

    if (std::remove("CUSTOMERdb.txt") != 0)
    {
        cerr << "Error deleting CUSTOMERdb.txt" << endl;
        cout << "Error deleting CUSTOMERdb.txt" << endl;
    }
    if (std::rename("temp.txt", "CUSTOMERdb.txt") != 0)
    {
        cerr << "Error in renaming CUSTOMERdb.txt" << endl;
        cout << "Error in renaming CUSTOMERdb.txt" << endl;
    }
    // remove("CUSTOMERdb.txt");
    // rename("temp.txt", "CUSTOMERdb.txt");
    cout << "Customer database updated successfully" << endl;
}

void Customer::browseMyCars()
{
    string cars = this->rentedCars;
    int num = 1;
    if (cars == "NONE")
    {
        cout << "You have not rented any cars currently." << endl;
    }
    else
    {
        cout << "Here are the cars you are currently renting: " << endl;
        for (char &ch : cars)
        {
            cout << num++ << "."
                 << " " << ch << endl;
        }
    }
}

void Customer::rentCar()
{
    // loop1:
    if (this->fine > 0)
    {
        cout << "You have a fine of " << fine << endl;
        cout << "Please clear your entire fine to rent a car." << endl;
        cout << "Choose one of the following actions:" << endl;
        cout << "1. Pay fine\t";
        cout << "2. Go back\t";
        cout << "3. Exit" << endl;
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            updateFine(0);
            update_database();
            cout << "Your fine has been updated." << endl;
            break;
        }
        case 2:
        {
            // Go back.
            return;
        }
        case 3:
        {
            // Exit.
            cout << "Exiting the program..." << endl;
            exit(0);
        }
        default:
        {
            cout << "Invalid choice." << endl;
            break;
        }
        }
        // goto loop1;
    }
    else
    {
        if ((this->customer_record) / 10 - this->numberOfRentedCars() <= 0)
        {
            cout << "Your record too poor to get access to rent another car. So, you cannot rent more than " << this->numberOfRentedCars() << " cars." << endl;
            return;
        }
        cout << "According to your record and number of cars you rented currently, your can rent a max of " << (this->customer_record) / 10 - this->numberOfRentedCars() << " cars." << endl;
        viewAvailableCars();
        cout << "Enter the model of the car you wish to rent :" << endl;
        char model;
        cin >> model;
        cout << "How many days do you want to rent this car ?" << endl;
        int days;
        cin >> days;
        if (days < 0)
        {
            cout << "Please enter valid input" << endl;
            return;
        }
        ifstream cardb;
        cardb.open("CARdb.txt");
        string line;
        bool isfound = false;
        cin.ignore();
        while (getline(cardb, line))
        {
            stringstream data(line);
            char Model;
            int Condition;
            string Availability;
            int Price;
            int dueDate;
            string rentedTo;
            data >> Model >> Condition >> Availability >> Price >> dueDate >> rentedTo;
            if (Model == model)
            {
                if (Availability == "Yes")
                {
                    isfound = true;
                    cout << "It costs you " << Price * days << ". Do you want to confirm the order?" << endl;
                    cout << "1. Yes" << endl;
                    cout << "2. No/Go back" << endl;
                    int choice;
                    cin >> choice;
                    if (choice == 1)
                    {
                        cout << "Car " << Model << " is now rented with return date due of " << days << " days" << endl;
                        Car C(Model, Condition, "No", Price, days, this->ID);
                        cardb.close();
                        C.update_database();
                        this->addRentedCars(Model);
                        this->update_database();
                    }
                    else
                    {
                        cardb.close();
                        return;
                    }
                }
                else
                {
                    cout << "Sorry, this car is not available. Choose one from available models" << endl;
                    cardb.close();
                    return;
                    // goto loop1;
                }
            }
        }
        if (!isfound)
        {
            cout << "This model does not exist!!" << endl;
            cardb.close();
            // goto loop1;
            return;
        }
    }
}

void Customer::returnCar()
{
    // The customer will enter the damage caused and number of days used.
    if (this->numberOfRentedCars() != 0)
    {
        cout << "Select the index of a car that you want to return." << endl;
        for (int i = 0; i < rentedCars.size(); i++)
        {
            cout << i + 1 << ". " << rentedCars[i] << endl;
        }
        int choice;
        // bool validChoice = false;
        cin >> choice;
        //****************************************************************
        if (choice > rentedCars.size())
        {
            cout << "Invalid choice" << endl;
            return;
        }
        cout << "Your choice is " << rentedCars[choice - 1] << endl;
        int damage;
        int daysUsed;
        cout << "Enter the damage caused and number of days used." << endl;
        cin >> damage >> daysUsed;
        if (damage < 0 || daysUsed < 0)
        {
            cout << "Enter valid inputs" << endl;
            return;
        }
        ifstream cardb;
        cardb.open("CARdb.txt");
        string line;
        cin.ignore();
        while (getline(cardb, line))
        {
            stringstream data(line);
            char Model;
            int Condition;
            string Availability;
            int Price;
            int DueDate;
            string rentedTo;
            data >> Model >> Condition >> Availability >> Price >> DueDate >> rentedTo;
            if (Model == rentedCars[choice - 1])
            {
                // validChoice = true;
                // update CUSTOMERdb(rentedCars, fine, Customer_record)
                // if its rented, there will be a due date.(i.e., it cant be *)
                // remove from rentedCars list.
                this->removeRentedCars(Model);
                int excessdaysUsed = daysUsed - DueDate;
                excessdaysUsed = max(excessdaysUsed, 0);
                cout << "excess days used: " << excessdaysUsed << ". So your fine is " << excessdaysUsed * Price << endl;
                this->updateFine(excessdaysUsed * Price);
                this->updateCustomerRecord(damage, excessdaysUsed);
                this->update_database();

                // Whenerver, car is returned, it is assumed that car will be repaired (damage is set to 0).
                // update CARdb(damage,availability, dueDate)
                Car C(Model, 0, "Yes", Price, 0, "N/A");
                cardb.close();
                C.update_database();
                break;
            }
        }
        // if (!validChoice)
        // {
        //     cout << "You did not rent this car!!" << endl;
        //     cardb.close();
        //     return;
        // }
        // else
        // {
        cout << "Car is returned successfully!!" << endl;
        return;
        // }
    }
    else
    {
        cout << "You do not have any rented cars to return!" << endl;
        return;
    }
}

class Employee : public User
{
private:
    int employee_record;
    int fine;
    string rentedCars;

public:
    static int number_of_employees;
    static int average_employee_record;
    Employee(string Name, string ID, string password, string RentedCars, int fine, int Employee_record)
    {
        this->Name = Name;
        this->ID = ID;
        this->password = password;
        this->employee_record = Employee_record;
        this->fine = fine;
        this->rentedCars = RentedCars;
    }

    void employeeSignup();
    int numberOfRentedCars()
    {
        if (this->rentedCars == "NONE")
            return 0;
        else
            return this->rentedCars.size();
    }
    void addRentedCars(char &car)
    {
        if (this->rentedCars == "NONE")
            this->rentedCars = car;
        // Add the rented cars to already rented cars of the employee.
        else
            this->rentedCars += car;
    }

    void removeRentedCars(char cars)
    {
        // Remove the rented cars from already rented cars of the customer.
        string carList = this->rentedCars;
        carList.erase(std::remove_if(carList.begin(), carList.end(), [cars](char c)
                                     { return c == cars; }),
                      carList.end());
        this->rentedCars = carList;
    }

    void updateEmployeeRecord(int damage, int excessdaysUsed)
    {
        // Each time, some customer's record is changed, average customer record will be updated.
        int total_employee_record = (number_of_employees * average_employee_record - this->employee_record);
        // Making sure that customer record is non-negative.
        if (excessdaysUsed > 0)
            this->employee_record = max(this->employee_record - (damage * 10) - (excessdaysUsed * 2), 0);
        else
            this->employee_record = max(this->employee_record - (damage * 10), 0);
        average_employee_record = (total_employee_record + this->employee_record) / (number_of_employees);
    }

    void updateFine(int fine)
    {
        this->fine = fine;
    }

    void update_database();

    // void viewAvailableCars(); -----> already present in userclass.

    void browseMyCars();

    void rentCar();

    void returnCar();

    void Interface();
};

int Employee::number_of_employees = 0;
int Employee::average_employee_record = AVG_RECORD;

void Employee::employeeSignup()
{
    // First increment the number of customers.
    number_of_employees++;
    string database = "EMPLOYEEdb.txt";
    fstream fdb;
    fdb.open(database, ios::app);
    if (fdb)
    {
        fdb << ID << " " << this->Name << " " << this->password << " "
            << "NONE"
            << " "
            << "0"
            << " " << this->employee_record << endl;
        fdb.close();
        cout << "Registerd successfully!!" << endl;
        cout << "Your ID is " << ID << endl;
    }
    else
    {
        cerr << "Error in opening Employee Database" << endl;
        cout << "Error in opening Employee Database" << endl;
        exit(1);
    }
}

void Employee::Interface()
{
loop:
    cout << "What would you like to do?" << endl;
    cout << "1. View available cars" << endl;
    cout << "2. Browse your current rented cars" << endl;
    cout << "3. Rent a car" << endl;
    cout << "4. Return a car" << endl;
    cout << "5. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        viewAvailableCars();
        break;
    }
    case 2:
    {
        // Browse the customer's cars.
        browseMyCars();
        break;
    }
    case 3:
    {
        // rent a car.
        rentCar();
        break;
    }
    case 4:
    {
        // return a car.
        // TODO
        returnCar();
        break;
    }
    case 5:
    {
        // Go back.
        return;
    }
    default:
    {
        cout << "Invalid choice." << endl;
        break;
    }
    }
    goto loop;
}

void Employee::update_database()
{
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("EMPLOYEEdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string id, name, pass, RentedCars;
        int Fine, Employee_record;
        data >> id >> name >> pass >> RentedCars >> Fine >> Employee_record;
        if (this->ID != id)
            newFile << id << " " << name << " " << pass << " " << RentedCars << " " << Fine << " " << Employee_record << endl;
        else
        {
            newFile << this->ID << " " << this->Name << " " << this->password << " " << this->rentedCars << " " << this->fine << " " << this->employee_record << endl;
        }
    }
    db.close();
    newFile.close();

    if (std::remove("EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error deleting EMPLOYEEdb.txt" << endl;
        cout << "Error deleting EMPLOYEEdb.txt" << endl;
    }
    if (std::rename("temp.txt", "EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error in renaming EMPLOYEEdb.txt" << endl;
        cout << "Error in renaming EMPLOYEEdb.txt" << endl;
    }
    // remove("EMPLOYEEdb.txt");
    // rename("temp.txt", "EMPLOYEEdb.txt");
    cout << "Employee database is updated successfully!!" << endl;
}

void Employee::browseMyCars()
{
    cout << "Here are the cars you are currently renting: " << endl;
    string cars = this->rentedCars;
    int num = 1;
    if (cars == "NONE")
    {
        cout << "Currently you have not rented any cars." << endl;
    }
    else
    {
        for (char &ch : cars)
        {
            cout << num++ << "."
                 << " " << ch << endl;
        }
    }
}

void Employee::rentCar()
{
    // loop:
    if (this->fine > 0)
    {
        cout << "You have a fine of " << this->fine << endl;
        cout << "Please pay entire your fine to rent a car." << endl;
        cout << "Choose one of the following actions:" << endl;
        cout << "1. Pay fine\t";
        cout << "2. Go back\t";
        cout << "3. Exit" << endl;
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            updateFine(0);
            update_database();
            cout << "Your fine has been updated." << endl;
            break;
        }
        case 2:
        {
            return;
        }
        case 3:
        {
            cout << "Exiting the program..." << endl;
            exit(0);
        }
        default:
        {
            cout << "Invalid choice." << endl;
            break;
        }
        }
        // goto loop;
    }
    else
    {
        if ((this->employee_record) / 10 - this->numberOfRentedCars() <= 0)
        {
            cout << "Your record too poor to get access to rent another car. So, you cannot rent more than " << this->numberOfRentedCars() << " cars." << endl;
            return;
        }
        cout << "According to your record and number of cars rented already, your can rent a max of " << (this->employee_record) / 10 - this->numberOfRentedCars() << " cars." << endl;
        viewAvailableCars();
        cout << "Enter the model of the car you wish to rent :" << endl;
        char model;
        cin >> model;
        cout << "How many days do you want to rent this car ?" << endl;
        int days;
        cin >> days;
        if (days < 0)
        {
            cout << "Please enter valid input" << endl;
            return;
        }
        ifstream cardb;
        cardb.open("CARdb.txt");
        string line;
        bool isfound = false;
        cin.ignore();
        while (getline(cardb, line))
        {
            stringstream data(line);
            char Model;
            int Condition;
            string Availability;
            int Price;
            int dueDate;
            string rentedTo;
            data >> Model >> Condition >> Availability >> Price >> dueDate >> rentedTo;
            if (Model == model)
            {
                if (Availability == "Yes")
                {
                    isfound = true;
                    cout << "It costs you " << (1 - DISCOUNT) * (Price * days) << "(Discounted). Do you want to confirm the rent?" << endl;
                    cout << "1. Yes" << endl;
                    cout << "2. No/Go back" << endl;
                    int choice;
                    cin >> choice;
                    if (choice == 1)
                    {
                        cout << "Car " << Model << " is now rented with return date due of " << days << " days" << endl;
                        Car C(Model, Condition, "No", Price, days, this->ID);
                        cardb.close();
                        C.update_database();
                        this->addRentedCars(Model);
                        this->update_database();
                    }
                    else
                    {
                        cardb.close();
                        return;
                    }
                }
                else
                {
                    cout << "Sorry, this car is not available. Choose one from available models" << endl;
                    cardb.close();
                    return;
                    // goto loop;
                }
            }
        }
        if (!isfound)
        {
            cout << "This model does not exist!!" << endl;
            cardb.close();
            // goto loop;
            return;
        }
    }
}

void Employee::returnCar()
{
    if (this->numberOfRentedCars() != 0)
    {
        cout << "Select the index of the car that you want to return." << endl;
        for (int i = 0; i < rentedCars.size(); i++)
        {
            cout << i + 1 << ". " << rentedCars[i] << endl;
        }
        int choice;
        // bool validChoice = false;
        cin >> choice;
        if (choice > rentedCars.size())
        {
            cout << "Invalid choice" << endl;
            return;
        }
        cout << "Your choice is " << rentedCars[choice - 1] << endl;
        int damage;
        int daysUsed;
        cout << "Enter the damage caused and number of days used." << endl;
        cin >> damage >> daysUsed;
        if (damage < 0 || daysUsed < 0)
        {
            cout << "Enter valid inputs" << endl;
            return;
        }
        ifstream cardb;
        cardb.open("CARdb.txt");
        string line;
        cin.ignore();
        while (getline(cardb, line))
        {
            stringstream data(line);
            char Model;
            int Condition;
            string Availability;
            int Price;
            int DueDate;
            string rentedTo;
            data >> Model >> Condition >> Availability >> Price >> DueDate >> rentedTo;
            if (Model == rentedCars[choice - 1])
            {
                // validChoice = true;
                // update CUSTOMERdb(rentedCars, fine, Customer_record)
                // if its rented, there will be a due date.(i.e., it cant be *)
                // remove from rentedCars list.
                this->removeRentedCars(Model);
                int excessdaysUsed = daysUsed - DueDate;
                excessdaysUsed = max(excessdaysUsed, 0);
                cout << "excess days used: " << excessdaysUsed << ". So your fine is " << excessdaysUsed * Price << endl;
                this->updateFine(excessdaysUsed * Price);
                this->updateEmployeeRecord(damage, excessdaysUsed);
                this->update_database();

                // Whenerver, car is returned, it is assumed that car will be repaired (damage is set to 0).
                // update CARdb(damage,availability, dueDate)
                Car C(Model, 0, "Yes", Price, 0, "N/A");
                cardb.close();
                C.update_database();
                break;
            }
        }
        // if (!validChoice)
        // {
        //     cout << "You did not rent this car!!" << endl;
        //     cardb.close();
        //     return;
        // }
        // else
        // {
        cout << "Car is returned successfully!!" << endl;
        return;
        // }
    }
    else
    {
        cout << "You do not have any rented cars to return!" << endl;
        return;
    }
}

class Manager : public User
{
    // A manager can add/update/delete any customer or employee.
    // Can also update/delete/add cars in the store.
    // Can see all the cars and who they are rented to.
private:
public:
    Manager(string ID, string Name, string password)
    {
        this->ID = ID;
        this->Name = Name;
        this->password = password;
    }

    void Interface();

    void addCustomer();
    void addEmployee();
    void addCar();

    void updateCustomer();
    void updateEmployee();
    void updateCar();

    void deleteCustomer();
    void deleteEmployee();
    void deleteCar();
    void viewCarsInfo();
};

void Manager::Interface()
{
loop:
    cout << "Choose one of the following actions: " << endl;
    cout << "1. Add a customer" << endl;
    cout << "2. Update a customer" << endl;
    cout << "3. Delete a customer" << endl;
    cout << "4. Add an employee" << endl;
    cout << "5. Update an employee" << endl;
    cout << "6. Delete an employee" << endl;
    cout << "7. Add a car" << endl;
    cout << "8. Update a car" << endl;
    cout << "9. Delete a car" << endl;
    cout << "10. View Cars rent info" << endl;
    cout << "11. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        addCustomer();
        break;
    }
    case 2:
    {
        updateCustomer();
        break;
    }
    case 3:

    {
        deleteCustomer();
        break;
    }
    case 4:
    {
        addEmployee();
        break;
    }
    case 5:
    {
        updateEmployee();
        break;
    }
    case 6:
    {
        deleteEmployee();
        break;
    }
    case 7:
    {
        addCar();
        break;
    }
    case 8:
    {
        updateCar();
        break;
    }
    case 9:
    {
        deleteCar();
        break;
    }
    case 10:
    {
        viewCarsInfo();
        break;
    }
    case 11:
    {
        // go back.
        return;
    }
    default:
    {
        cout << "Invalid choice" << endl;
        break;
    }
    }
    goto loop;
}

void Manager::addCustomer()
{
    // ID will be decided by default.---> cant be given by manager.
    cout << "Enter the details of the customer: " << endl;
    string ID;
    string Name;
    string password;
    cout << "Enter the name of the customer: " << endl;
    cin >> Name;
    cout << "Enter the password of the customer: " << endl;
    cin >> password;
    ifstream db;
    db.open("CUSTOMERdb.txt");
    int id = nextID(db);
    db.close();
    ID = to_string(id) + 'C';
    Customer C(Name, ID, password, "NONE", 0, Customer::average_customer_record);
    C.customerSignup();
}

void Manager::updateCustomer()
{
    cout << "Enter the ID of the customer that you want to edit: " << endl;
    string id;
    cin >> id;
    bool validID = false;
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("CUSTOMERdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string ID, Name, Password, RentedCars;
        int Fine, Customer_record;
        data >> ID >> Name >> Password >> RentedCars >> Fine >> Customer_record;
        if (ID != id)
            newFile << ID << " " << Name << " " << Password << " " << RentedCars << " " << Fine << " " << Customer_record << endl;
        else
        {
            validID = true;
            cout << "The following is this customer's info. Just enter the data to be changed in same order and enter the same data if you do not want to change a particular attribute." << endl;
            cout << "[NOTE: Manager can't edit any customer's rentedCars attribute.(NO ACCESS)]" << endl;
            cout << "Name: " << Name << endl;
            cout << "Password: " << Password << endl;
            cout << "Fine: " << Fine << endl;
            cout << "Customer_Record: " << Customer_record << endl;
            string name, password;
            int fine, customerRecord;
            cin >> name >> password >> fine >> customerRecord;
            // Changing the average customer record.
            int total_record = (Customer::number_of_customers * Customer::average_customer_record - customerRecord);
            Customer::average_customer_record = (total_record + customerRecord) / (Customer::number_of_customers);

            newFile << ID << " " << name << " " << password << " " << RentedCars << " " << fine << " " << customerRecord << endl;
            cout << "Customer updated successfully!!" << endl;
        }
    }
    db.close();
    newFile.close();
    if (!validID)
    {
        cout << "This customerID does not exist" << endl;
    }

    if (std::remove("CUSTOMERdb.txt") != 0)
    {
        cerr << "Error deleting CUSTOMERdb.txt" << endl;
        cout << "Error deleting CUSTOMERdb.txt" << endl;
    }
    if (std::rename("temp.txt", "CUSTOMERdb.txt") != 0)
    {
        cerr << "Error in renaming CUSTOMERdb.txt" << endl;
        cout << "Error in renaming CUSTOMERdb.txt" << endl;
    }
}

void Manager::deleteCustomer()
{
    cout << "Enter the customer ID that you want to delete: " << endl;
    string id;
    cin >> id;
    bool validID = false;
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("CUSTOMERdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string ID, Name, password, rentedCars;
        int fine, customer_record;
        data >> ID >> Name >> password >> rentedCars >> fine >> customer_record;
        if (ID != id)
            newFile << ID << " " << Name << " " << password << " " << rentedCars << " " << fine << " " << customer_record << endl;
        else
        {
            validID = true;
            if (rentedCars != "NONE")
            {
                cout << "You cannot delete this customer because he has rented " << rentedCars << endl;
                newFile << ID << " " << Name << " " << password << " " << rentedCars << " " << fine << " " << customer_record << endl;
                // break;
                continue;
            }
            // Decrement the number of customers in customer class and change the average customer record.
            Customer::average_customer_record = ((Customer::average_customer_record) * (Customer::number_of_customers)-customer_record) / (Customer::number_of_customers - 1);
            Customer::number_of_customers--;
        }
    }
    db.close();
    newFile.close();
    if (!validID)
    {
        cout << "This customerID does not exist" << endl;
    }

    if (std::remove("CUSTOMERdb.txt") != 0)
    {
        cerr << "Error deleting CUSTOMERdb.txt" << endl;
        cout << "Error deleting CUSTOMERdb.txt" << endl;
    }
    if (std::rename("temp.txt", "CUSTOMERdb.txt") != 0)
    {
        cerr << "Error in renaming CUSTOMERdb.txt" << endl;
        cout << "Error in renaming CUSTOMERdb.txt" << endl;
    }
}

void Manager::addEmployee()
{
    // ID will be decided by default.---> cant be given by manager.
    cout << "Enter the details of the employee: " << endl;
    string ID;
    string Name;
    string password;
    cout << "Enter the name of the employee: " << endl;
    cin >> Name;
    cout << "Enter the password of the employee: " << endl;
    cin >> password;
    ifstream db;
    db.open("EMPLOYEEdb.txt");
    int id = nextID(db);
    db.close();
    ID = to_string(id) + 'E';
    Employee E(Name, ID, password, "NONE", 0, Employee::average_employee_record);
    E.employeeSignup();
}

void Manager::updateEmployee()
{
    cout << "Enter the ID of the employee that you want to edit: " << endl;
    string id;
    cin >> id;
    bool validID = false;
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("EMPLOYEEdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string ID, Name, Password, RentedCars;
        int Fine, Employee_record;
        data >> ID >> Name >> Password >> RentedCars >> Fine >> Employee_record;
        if (ID != id)
            newFile << ID << " " << Name << " " << Password << " " << RentedCars << " " << Fine << " " << Employee_record << endl;
        else
        {
            validID = true;
            // newFile << this->ID << " " << this->Name << " " << this->password << " " << this->rentedCars << " " << this->fine << " " << this->customer_record << endl;
            cout << "The following is this customer's info. Just enter the data to be changed in same order and enter the same data if you do not want to change a particular attribute." << endl;
            cout << "[NOTE: Manager can't edit any employee's rentedCars attribute.(NO ACCESS)]" << endl;
            // cout << Name << " " << Password << " " << RentedCars << " " << Fine << " " << Employee_record << endl;
            cout << "Name: " << Name << endl;
            cout << "Password: " << Password << endl;
            // cout << "RentedCars: " << RentedCars << endl;
            cout << "Fine: " << Fine << endl;
            cout << "Employee_Record: " << Employee_record << endl;
            string name, password;
            int fine, employeeRecord;
            cin >> name >> password >> fine >> employeeRecord;
            // Changing average record.
            int total_record = (Employee::number_of_employees * Employee::average_employee_record - Employee_record);
            Employee::average_employee_record = (total_record + employeeRecord) / (Employee::number_of_employees);

            newFile << ID << " " << name << " " << password << " " << RentedCars << " " << fine << " " << employeeRecord << endl;
            cout << "Employee updated successfully!!" << endl;
        }
    }
    db.close();
    newFile.close();
    if (!validID)
    {
        cout << "This employeeID does not exist" << endl;
    }

    if (std::remove("EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error deleting EMPLOYEEdb.txt" << endl;
        cout << "Error deleting EMPLOYEEdb.txt" << endl;
    }
    if (std::rename("temp.txt", "EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error in renaming EMPLOYEEdb.txt" << endl;
        cout << "Error in renaming EMPLOYEEdb.txt" << endl;
    }
}

void Manager::deleteEmployee()
{
    cout << "Enter the employee ID that you want to delete: " << endl;
    string id;
    cin >> id;
    bool validID = false;
    ofstream newFile;
    newFile.open("temp.txt");
    ifstream db;
    db.open("EMPLOYEEdb.txt");
    string line;
    while (getline(db, line))
    {
        stringstream data(line);
        string ID, Name, password, rentedCars;
        int fine, employee_record;
        data >> ID >> Name >> password >> rentedCars >> fine >> employee_record;
        if (ID != id)
            newFile << ID << " " << Name << " " << password << " " << rentedCars << " " << fine << " " << employee_record << endl;
        else
        {
            validID = true;
            if (rentedCars != "NONE")
            {
                cout << "You cannot delete this employee because he has rented " << rentedCars << endl;
                newFile << ID << " " << Name << " " << password << " " << rentedCars << " " << fine << " " << employee_record << endl;
                // break;
                continue;
            }
            // Decrement the number of customers in customer class and change the average customer record.
            Employee::average_employee_record = ((Employee::average_employee_record) * (Employee::number_of_employees)-employee_record) / (Employee::number_of_employees - 1);
            Employee::number_of_employees--;
        }
    }
    db.close();
    newFile.close();
    if (!validID)
    {
        cout << "This employeeID does not exist" << endl;
    }

    if (std::remove("EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error deleting EMPLOYEEdb.txt" << endl;
        cout << "Error deleting EMPLOYEEdb.txt" << endl;
    }
    if (std::rename("temp.txt", "EMPLOYEEdb.txt") != 0)
    {
        cerr << "Error in renaming EMPLOYEEdb.txt" << endl;
        cout << "Error in renaming EMPLOYEEdb.txt" << endl;
    }
}

void Manager::addCar()
{
    cout << "Give the car details here in the same order as below (mention N/A in Customer/Employee ID attribute if no one is currently renting it): " << endl;
    cout << "[Make sure that model is unique(i.e., different from all that of previous cars)]"<<endl;
    cout << "Model Condition RentCost(per day)" << endl;
    char Model;
    int Condition, RentCost;
    // string Availability, ID;
    cin >> Model >> Condition >>RentCost;
    fstream fdb;
    fdb.open("CARdb.txt", ios::app);
    if (fdb)
    {
        fdb << Model << " " << Condition << " " << "Yes" << " "
            << RentCost
            << " "
            << 0 << " " << "N/A" << endl;
        fdb.close();
        cout << "Added successfully!!" << endl;
    }
    else
    {
        cerr << "Error in opening Car Database" << endl;
        cout << "Error in opening Car Database" << endl;
        exit(1);
    }
}

void Manager::updateCar()
{
    cout << "Enter the Car model that you want to update" << endl;
    char input_model;
    cin >> input_model;

    ofstream NewFile;
    NewFile.open("car.txt");
    ifstream car_db;
    car_db.open("CARdb.txt");
    string line;
    while (getline(car_db, line))
    {
        stringstream data(line);
        char model;
        int condition;
        string availability;
        int price;
        int dueDate;
        string rentedTo;
        data >> model >> condition >> availability >> price >> dueDate >> rentedTo;
        if (input_model != model)
            NewFile << model << " " << condition << " " << availability << " " << price << " " << dueDate << " " << rentedTo << endl;
        else
        {
            cout << "The following is this car's info. Just enter the data to be changed in same order and enter the same data if you do not want to change a particular attribute." << endl;
            cout << "Condition: " << condition << endl;
            // cout << "Availability: " << availability << endl;
            cout << "Price(per day)" << price << endl;
            cout << "DueDate" << dueDate << endl;
            // cout << "RentedTo" << rentedTo << endl;
            int input_condition;
            // string input_availability;
            int input_price;
            int input_dueDate;
            // string input_rentedTo;
            cin >> input_condition >> input_price >> input_dueDate;
            NewFile << model << " " << input_condition << " " << availability << " " << input_price << " " << input_dueDate << " " << rentedTo << endl;
            cout << "Car updated successfully!!" << endl;
        }
    }
    car_db.close();
    NewFile.close();

    if (std::remove("CARdb.txt") != 0)
    {
        // cerr << "Error deleting" << endl;
        cout << "Error deleting CARdb.txt" << endl;
    }
    if (std::rename("car.txt", "CARdb.txt") != 0)
    {
        // cerr << "Error in renaming" << endl;
        cout << "Error in renaming car.txt" << endl;
    }
    cout << "Car database updated successfully!!" << endl;
}

void Manager::deleteCar()
{
    cout << "Enter the model you want to delete: " << endl;
    char input_model;
    cin >> input_model;
    ofstream NewFile;
    NewFile.open("car.txt");
    ifstream car_db;
    car_db.open("CARdb.txt");
    string line;
    while (getline(car_db, line))
    {
        stringstream data(line);
        char model;
        int condition;
        string availability;
        int price;
        int dueDate;
        string rentedTo;
        data >> model >> condition >> availability >> price >> dueDate >> rentedTo;
        if (input_model != model)
            NewFile << model << " " << condition << " " << availability << " " << price << " " << dueDate << " " << rentedTo << endl;
        else
        {
            if (rentedTo != "N/A")
            {
                cout << "You cannot delete this car because it is currently rented by " << rentedTo << endl;
                NewFile << model << " " << condition << " " << availability << " " << price << " " << dueDate << " " << rentedTo << endl;
                // break;
            }
            // Do nothing(i.e., skip)
        }
    }
    car_db.close();
    NewFile.close();

    if (std::remove("CARdb.txt") != 0)
    {
        cerr << "Error deleting" << endl;
        cout << "Error deleting CARdb.txt" << endl;
    }
    if (std::rename("car.txt", "CARdb.txt") != 0)
    {
        cerr << "Error in renaming" << endl;
        cout << "Error in renaming car.txt" << endl;
    }
    cout << "Car database updated successfully!!" << endl;
}

void Manager::viewCarsInfo()
{
    ifstream db;
    db.open("CARdb.txt");
    string line;
    int num = 1;
    if (db)
    {
        cout << "Here is the information about all the cars and to whom they are rented to:" << endl;
        while (getline(db, line))
        {
            stringstream data(line);
            char model;
            int condition;
            string availability;
            int price;
            int dueDate;
            string rentedTo;
            data >> model >> condition >> availability >> price >> dueDate >> rentedTo;
            if (rentedTo != "N/A")
                cout << num++ << ". " << model << " is rented to " << rentedTo << endl;
            else
                cout << num++ << ". " << model << " is not rented" << endl;
        }
        db.close();
    }
    else
    {
        cerr << "Error in opening Car Database" << endl;
        cout << "Error in opening Car Database" << endl;
        exit(1);
    }
}

Customer loginCustomer(string database)
{
loop:
    string id;
    string password;
    string line;
    cout << "Enter your ID: \n";
    cin >> id;
    cout << "Enter your password: \n";
    cin >> password;
    // Bool variable to check whether ID is valid.
    bool isMatched = false;
    ifstream db;
    db.open(database);

    while (getline(db, line))
    {
        stringstream data(line);
        string ID;
        string Name;
        string Password;
        string RentedCars;
        int FineDue;
        int Record;
        data >> ID >> Name >> Password >> RentedCars >> FineDue >> Record;
        if (id == ID)
        {
            isMatched = true;
            if (password == Password)
            {
                cout << "Login successful" << endl;
                cout << "Welcome " << Name << "!" << endl;
                Customer C(Name, ID, Password, RentedCars, FineDue, Record);
                db.close();
                return C;
            }
            else
            {
                cout << "Wrong password" << endl;
                db.close();
                goto loop;
            }
        }
    }
    if (!isMatched)
    {
        cout << "ID not found" << endl;
        db.close();
        goto loop;
    }
}

Customer SignupCustomer(string database)
{
    string name;
    string id;
    string password;
    string line;
    cout << "Enter your name: \n";
    cin >> name;
    cout << "Enter your password: \n";
    cin >> password;
    ifstream db;
    db.open(database);
    int num = nextID(db);
    db.close();
    string ID = to_string(num) + 'C';
    // Initially, the customer record is set to zero here.
    cout << "customer ID is: " << ID << endl;
    Customer C(name, ID, password, "NONE", 0, Customer::average_customer_record);
    // Now, we update the customer record to average the customer record and add to customer database.
    C.customerSignup();
    return C;
}

Employee loginEmployee(string database)
{
loop:
    string id;
    string password;
    string line;
    cout << "Enter your ID: \n";
    cin >> id;
    cout << "Enter your password: \n";
    cin >> password;
    // Bool variable to check whether ID is valid.
    bool isMatched = false;
    ifstream db;
    db.open(database);

    while (getline(db, line))
    {
        stringstream data(line);
        string ID;
        string Name;
        string Password;
        string RentedCars;
        int FineDue;
        int Record;
        data >> ID >> Name >> Password >> RentedCars >> FineDue >> Record;
        if (id == ID)
        {
            isMatched = true;
            if (password == Password)
            {
                cout << "Login successful" << endl;
                cout << "Welcome " << Name << "!" << endl;
                Employee E(Name, ID, Password, RentedCars, FineDue, Record);
                db.close();
                return E;
            }
            else
            {
                cout << "Wrong password" << endl;
                db.close();
                goto loop;
            }
        }
    }
    if (!isMatched)
    {
        cout << "ID not found" << endl;
        db.close();
        goto loop;
    }
}

Employee SignupEmployee(string database)
{
    string name;
    string id;
    string password;
    string line;
    cout << "Enter your name: \n";
    cin >> name;
    cout << "Enter your password: \n";
    cin >> password;
    ifstream db;
    db.open(database);
    int num = nextID(db);
    db.close();
    string ID = to_string(num) + 'E';
    // Initially, the employee record is set to zero here.
    Employee E(name, ID, password, "NONE", 0, Employee::average_employee_record);
    // Now, we update the employee record to average the employee record and add to employee database.
    E.employeeSignup();
    return E;
}

Manager loginManager(string database)
{
loop:
    string id;
    string password;
    string line;
    cout << "Enter your ID: \n";
    cin >> id;
    cout << "Enter your password: \n";
    cin >> password;
    // Bool variable to check whether ID is valid.
    bool isMatched = false;
    ifstream db;
    db.open(database);

    while (getline(db, line))
    {
        stringstream data(line);
        string ID;
        string Name;
        string Password;
        // string RentedCars;
        // int FineDue;
        // int Record;
        data >> ID >> Name >> Password;
        if (id == ID)
        {
            isMatched = true;
            if (password == Password)
            {
                cout << "Login successful" << endl;
                cout << "Welcome " << Name << "!" << endl;
                Manager M(ID, Name, Password);
                db.close();
                return M;
            }
            else
            {
                cout << "Wrong password" << endl;
                db.close();
                goto loop;
            }
        }
    }
    if (!isMatched)
    {
        cout << "ID not found" << endl;
        db.close();
        goto loop;
    }
}

void CustomerHandle()
{
loop:
    cout << "Choose one of the following actions: " << endl;
    cout << "1. login\t"
         << "2. Signup\t"
         << "3. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        // Login
        Customer C = loginCustomer("CUSTOMERdb.txt");
        C.Interface();
        break;
    }
    case 2:
    {
        // Signup
        Customer C = SignupCustomer("CUSTOMERdb.txt");
        C.Interface();
        break;
    }
    case 3:
    {
        // Go back
        return;
    }
    default:
    {
        cout << "Invalid choice" << endl;
        break;
    }
    }
    goto loop;
}

void EmployeeHandle()
{
loop:
    cout << "Choose one of the following actions: " << endl;
    cout << "1. login\t"
         << "2. Signup\t"
         << "3. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        // Login
        Employee E = loginEmployee("EMPLOYEEdb.txt");
        E.Interface();
        break;
    }
    case 2:
    {
        // Signup
        Employee E = SignupEmployee("EMPLOYEEdb.txt");
        E.Interface();
        break;
    }
    case 3:
    {
        // Go back
        return;
    }
    default:
    {
        cout << "Invalid choice" << endl;
        break;
    }
    }
    goto loop;
}

void ManagerHandle()
{
loop:
    cout << "Choose one of the following actions: " << endl;
    cout << "1. login\t"
         << "2. Go back" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        // Login
        Manager M = loginManager("MANAGERdb.txt");
        M.Interface();
        break;
    }
    case 2:
    {
        // Go back.
        return;
    }
    default:
    {
        cout << "Invalid choice" << endl;
        break;
    }
    }
    goto loop;
}

std::pair<int, int> getAverageRecord(string database)
{
    int sum = 0;
    int count = 0;
    // int maxID = 0;
    string line;
    ifstream db;
    db.open(database);
    while (getline(db, line))
    {
        stringstream data(line);
        string ID;
        string Name;
        string Password;
        string RentedCars;
        int FineDue;
        int Record;
        data >> ID >> Name >> Password >> RentedCars >> FineDue >> Record;
        sum += Record;
        count++;
        // maxID = max(maxID,ID[0] - '0');
    }
    db.close();
    // Customer::average_customer_record = sum/count;

    return std::make_pair(sum / count, count);
}

int main()
{
    // Setting avg employee and customer record at the beginning.
    // 1. Customer.
    pair<int, int> customerDetails = getAverageRecord("CUSTOMERdb.txt");
    Customer::average_customer_record = customerDetails.first;
    Customer::number_of_customers = customerDetails.second;
    // cout<<"Average customer record is : "<<Customer::average_customer_record<<endl;
    // 2. Employee.
    pair<int, int> employeeDetails = getAverageRecord("EMPLOYEEdb.txt");
    Employee::average_employee_record = employeeDetails.first;
    Employee::number_of_employees = employeeDetails.second;
    // cout<<"Average employee record is : "<<Employee::average_employee_record<<endl;

loop:
    cout << "Welcome to the CAR RENTAL SYSTEM" << endl;
    cout << "Choose one of the following actions: " << endl;
    cout << "1.Customer\t";
    cout << "2.Employee\t";
    cout << "3.Manager\t";
    cout << "4.Exit" << endl;
    int action;
    cin >> action;
    switch (action)
    {
    case 1:
    {
        // Customer
        CustomerHandle();
        break;
    }
    case 2:
    {
        // Employee
        EmployeeHandle();
        break;
    }
    case 3:
    {
        // Manager
        ManagerHandle();
        break;
    }
    case 4:
    {
        // Exit
        cout << "Exiting the program..." << endl;
        exit(0);
    }
    default:
    {
        cout << "Invalid command!!" << endl;
        cout << "Please enter a valid command." << endl;
        break;
    }
    }
    goto loop;
    return 0;
}