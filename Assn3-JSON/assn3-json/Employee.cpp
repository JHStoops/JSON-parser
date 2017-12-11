#include "Employee.h"

Employee::Employee(std::string _name, int _id, std::string _address, std::string _city, std::string _state, std::string _phone, double _salary)
	: name(_name), address(_address), city(_city), state(_state), phone(_phone) {
	this->id = _id;
	this->salary = _salary;
}

void Employee::display(std::ostream & os) const{
	os << "ID: " << this->id;
	os << "\nName: " << this->name;
	os << "\nAddress: " << this->address;
	os << "\nCity: " << this->city;
	os << "\nState: " << this->state;
	os << "\nPhone: " << this->phone;
	os << "\nSalary: " << this->salary << std::endl << std::endl;
}

void Employee::write(std::ostream& os) const{
	EmployeeRec tempEmp;
	tempEmp.id = this->id;
	strncpy(tempEmp.name, this->name.c_str(), 31)[31] = '\0';
	strncpy(tempEmp.address, this->address.c_str(), 31)[31] = '\0';
	strncpy(tempEmp.city, this->city.c_str(), 21)[21] = '\0';
	strncpy(tempEmp.state, this->state.c_str(), 21)[21] = '\0';
	strncpy(tempEmp.phone, this->phone.c_str(), 21)[21] = '\0';
	tempEmp.salary = this->salary;
	os.write(reinterpret_cast<char*>(&tempEmp), sizeof(tempEmp));
}

void Employee::store(std::iostream& is) const{
	//Find Employee object with matching ID
	Employee* emp = retrieve(is, this->id);
	is.clear();

	//Prepare fixed-length object to transfer data
	EmployeeRec tempEmp;
	tempEmp.id = this->id;
	strncpy(tempEmp.name, this->name.c_str(), 31)[31] = '\0';
	strncpy(tempEmp.address, this->address.c_str(), 31)[31] = '\0';
	strncpy(tempEmp.city, this->city.c_str(), 21)[21] = '\0';
	strncpy(tempEmp.state, this->state.c_str(), 21)[21] = '\0';
	strncpy(tempEmp.phone, this->phone.c_str(), 21)[21] = '\0';
	tempEmp.salary = this->salary;

	//Replace or append employee object in file
	if (emp == nullptr) is.seekp(is.tellg());				//Append
	else is.seekp(int(is.tellg()) - sizeof(EmployeeRec));	//Replace
	is.write(reinterpret_cast<char*>(&tempEmp), sizeof(tempEmp));
	is.flush();
}

void Employee::toJSON(std::ostream& os) const{
	const int FIRST_INDENT = 3;
	const int SECOND_INDENT = 6;
	std::cout << std::setw(FIRST_INDENT) << ' ' << JSON_OBJECT_OPEN << '\n';
	std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "Name" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << JSON_QUOTE << this->name << "\",\n";
	std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "ID" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << this->id << ",\n";
	if (this->address != "") std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "Address" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << JSON_QUOTE << this->address << "\",\n";
	if (this->city != "") std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "City" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << JSON_QUOTE << this->city << "\",\n";
	if (this->state != "") std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "State" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << JSON_QUOTE << this->state << "\",\n";
	if (this->phone != "") std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "Phone" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << JSON_QUOTE << this->phone << "\",\n";
	std::cout << std::setw(SECOND_INDENT) << ' ' << JSON_QUOTE << "Salary" << JSON_QUOTE << ' ' << JSON_KEY_VALUE_SEPARATOR << ' ' << this->salary << '\n';
	std::cout << std::setw(FIRST_INDENT) << ' ' << JSON_OBJECT_CLOSE;
}

double Employee::getSalary() const {
	return this->salary;
}

void Employee::setSalary(double _salary) {
	this->salary = _salary;
}

int Employee::getID() const {
	return this->id;
}