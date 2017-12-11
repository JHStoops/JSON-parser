#ifndef EMPLOYEE
#include <iostream>
#include <locale>		//to use tolower
#include <stdexcept>	//to use runtime_error
#include <cstring>		//to use strncpy
#include <string>		
#include <iomanip>
#include <memory>		//to use unique_ptr
#include <algorithm>
#include <cassert>

class Employee
{
private:
	std::string name;
	int id;
	std::string address;
	std::string city;
	std::string state;
	std::string phone;
	double salary;

public:
	static const char JSON_ARRAY_OPEN = '[';
	static const char JSON_ARRAY_CLOSE = ']';
	static const char JSON_OBJECT_OPEN = '{';
	static const char JSON_OBJECT_CLOSE = '}';
	static const char JSON_OBJECT_SEPARATOR = ',';
	static const char JSON_KEY_VALUE_SEPARATOR = ':';
	static const char JSON_QUOTE = '"';

	//For transferring data
	struct EmployeeRec {
		int id;
		char name[31];
		char address[31];
		char city[21];
		char state[21];
		char phone[21];
		double salary;
	};

	//Constructors
	Employee() = default;
	Employee(std::string, int, std::string, std::string, std::string, std::string, double);

	//Methods
	void display(std::ostream&) const;  // Write a readable Employee representation to a stream
	void write(std::ostream&) const;    // Write a fixed-length record to current file position
	void store(std::iostream&) const;   // Overwrite (or append) record in (to) file
	void toJSON(std::ostream&) const;   // Write JSON record for Employee
	double getSalary() const;
	void setSalary(double);
	int getID() const;

	//Static Methods
	static Employee* read(std::istream& is) {	// Read record from current file position wrap in smart ptr
		if (!is) return nullptr;

		EmployeeRec tempEmp;
		is.read(reinterpret_cast<char*>(&tempEmp), sizeof(tempEmp));

		Employee* emp = new Employee();
		emp->id = tempEmp.id;
		emp->name = tempEmp.name;
		emp->address = tempEmp.address;
		emp->city = tempEmp.city;
		emp->state = tempEmp.state;
		emp->phone = tempEmp.phone;
		emp->salary = tempEmp.salary;

		return emp;
	}
	static Employee* retrieve(std::istream& is, const int _id) { // Search file for record by id wrap in smart ptr
		//If id was found, it returns the Employee object. Else, it returns nullptr.
		Employee* emp = nullptr;
		while (is) {
			emp = Employee::read(is);
			if (emp->getID() == _id) break;
			delete emp;
			emp = nullptr;
		}
		return emp;
	}
	static std::string trim(const std::string& s) {
		//trims leading and trailing spaces and new lines
		int first = s.find_first_not_of(' ');
		if (first == std::string::npos) return s;
		else if (s.at(first) == '\n') first = s.find_first_not_of(' ', first + 1);

		std::string temp = s.substr(0, s.find_last_of('\n'));
		int last = temp.find_last_not_of(' ');
		return s.substr(first, (last - first + 1));
	}
	static void getNextKey(std::istream& is, std::string& key) {
		//Sets is' position to the first letter of valid Key

		std::string str = "";
		is.seekg(std::getline(is, str, JSON_KEY_VALUE_SEPARATOR).tellg());	//Grabs the data it needs
		
		//Check if there's a missing '"' on either end
		if (!(str.find(JSON_QUOTE) != std::string::npos) || !(str.find_last_of(JSON_QUOTE) != std::string::npos) || str.find(JSON_QUOTE) == str.find_last_of(JSON_QUOTE))
			throw std::runtime_error("Format error: Incomplete '\"' pair surrounding Key.");

		//strip of the '"'
		if (str.find(JSON_QUOTE)) {
			int firstQuote = str.find(JSON_QUOTE) + 1;
			int secondQuote = str.find_last_of(JSON_QUOTE);
			key = str.substr(firstQuote, secondQuote - firstQuote);
		}
		//Trim leading and trailing spaces
		key = trim(key);
	}
	static void getNextValue(std::istream& is, std::string& val, const std::string& key) {
		//Sets is' position to the first letter of valid Value

		//Check if end of file
		std::string str = "";
		is.seekg(std::getline(is, str, JSON_OBJECT_SEPARATOR).tellg());

		//Check if end of file
		if (!is) {
			if (str.find(JSON_ARRAY_CLOSE) == -1) throw std::runtime_error("Format error: Missing ']' to end the array of JSON objects.");
		}
		//Check if there's a missing '"' on either end
		if ( (str.find(JSON_QUOTE) > -1) != (str.find_last_of(JSON_QUOTE) > -1)) throw std::runtime_error("Format error: Incomplete '\"' pair surrounding Value.");
		//Check if numbers have quotation marks
		if ( (key == "id" || key == "salary") && str.find(JSON_QUOTE) != -1) throw std::runtime_error("Format error: Numerical value cannot be in quotations");
		else if ((key == "name" || key == "address" || key == "city" || key == "state" || key == "phone") && (str.find(JSON_QUOTE) == -1 || str.find(JSON_QUOTE) == str.find_last_of(JSON_QUOTE)) )
			throw std::runtime_error("Format error: String value must be in quotations.");
		//If value is a string, strip of the '"'
		if (str.find(JSON_QUOTE)) {
			int firstQuote = str.find(JSON_QUOTE) + 1;
			int secondQuote = str.find_last_of(JSON_QUOTE);
			val = str.substr(firstQuote, secondQuote - firstQuote);
		}
		//Strip off white space
		val = trim(val);
	}
	static Employee* fromJSON(std::istream& is) {      // Read the JSON record from a stream
		//if Fail
		if (is.eof()) return nullptr;
		assert(is);

		//Check for opening '[' if pointer is 0
		if (is.tellg().operator==(0)) {
			if (is.get() != JSON_ARRAY_OPEN)
				throw std::runtime_error("Format error: Array missing '['");
		}

		//Prapare for data retrieval
		std::string tempStr = "";
		is.seekg(std::getline(is, tempStr, JSON_OBJECT_OPEN).tellg());
		if (tempStr.find(JSON_ARRAY_OPEN) != -1) throw std::runtime_error("Format error: Using '[' to separate Employee objects.");
		Employee* emp = new Employee();
		std::string endOfObj = "", key = "", val = "";
		std::streampos startObj = is.tellg();
		std::streampos endObj = std::getline(is, endOfObj, JSON_OBJECT_CLOSE).tellg();
		if (endOfObj.find(JSON_ARRAY_CLOSE) != -1) throw std::runtime_error("Format error: Using ']' to separate Employee objects.");
		is.seekg(startObj);

		//Gather data from file to create a single EmployeeRec object
		int id = -1;
		while (is.tellg() >= startObj && is.tellg() < endObj) {
			Employee::getNextKey(is, key);
			Employee::getNextValue(is, val, key);

			//Make case-insensitive
			std::locale loc;
			for (auto& i : key)
				i = std::tolower(i, loc);

			//Copy value into EmployeeRec object
			if (key == "id") emp->id = std::stoi(val);
			else if (key == "salary") emp->salary = std::stod(val);
			else if (key == "name") emp->name = val;
			else if (key == "address") emp->address = val;
			else if (key == "city") emp->city = val;
			else if (key == "state") emp->state = val;
			else if (key == "phone") emp->phone = val;
			else throw std::runtime_error("Invalid Key.");
		}
		
		//Test name and id
		if (emp->name[0] == ' ') throw std::runtime_error("Key value required: Missing name.");
		if (emp->id == -1) throw std::runtime_error("Key value required: Missing ID.");

		//Make sure EmployeeRec object has at least name and id
		return emp;
	}
};

#endif // !EMPLOYEE