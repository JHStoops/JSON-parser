#include "Employee.h"
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <memory>

using namespace std;

int main(int argc, char* argv[]) {
	//Step 1: End program if no file is specified.
	if (argc == 1) {
		cout << "No file specified for read process." << endl;
		return 1;
	}

	//Define local variables
	fstream fs;
	ifstream ifs(argv[1], istream::in | istream::binary);
	assert(ifs);
	deque<unique_ptr<Employee>> emps;
	string binFile("emp.bin");

	//Step 2: Read JSON data from file, create individual Employee objects
	try {
		while (ifs)	emps.push_back(unique_ptr<Employee>(Employee::fromJSON(ifs)));
	}
	catch (const runtime_error& e) {
		cout << e.what() << endl;
		return 1;
	}
	ifs.close();

	//Step 3: Print out data from each Employee object
	for (const auto& emp : emps) emp->display(cout);

	//Step 4: Write fixed length Employee records to emp.bin
	fs.open(binFile, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	assert(fs);
	for (const auto& emp : emps) emp->write(fs);
	fs.close();

	//Step 5: clear deque
	emps.clear();

	//Step 6: Read emp.bin to recreate deque
	ifs.open(binFile, istream::in | istream::binary);
	assert(ifs);
	while (ifs) {
		//unique_ptr won't work here. It throws an "attempting to reference a deleted function."
		Employee* tempEmp = Employee::read(ifs);
		if (ifs.gcount() == sizeof(Employee::EmployeeRec)) emps.push_back(unique_ptr<Employee>(tempEmp));
		else delete tempEmp;
	}
	ifs.close();

	//Step 7: Print all Employee objects in JSON format
	cout << Employee::JSON_ARRAY_OPEN << '\n';
	for (const auto& emp : emps) {
		if (!(emp->getID() == emps[0]->getID())) cout << Employee::JSON_OBJECT_SEPARATOR << '\n';
		emp->toJSON(cout);
	}
	cout << '\n' << Employee::JSON_ARRAY_CLOSE << endl;

	//Step 8: Search file for Employee with id: 123456
	int id = 123456;
	ifs.open(binFile, istream::in | istream::binary);
	unique_ptr<Employee>emp(Employee::retrieve(ifs, id));
	ifs.close();

	//Step9: Change salary to 15,000.0 if Emplyee was found
	double salary = 15000;
	if (emp != nullptr) {
		cout << "Found:\n";
		emp->display(std::cout);
		emp->setSalary(salary);

		//Step 10: Write This employee object back into
		fs.open(binFile, fstream::in | fstream::out | fstream::binary);
		assert(fs);
		emp->store(fs);
		fs.close();

		//Step 11: Retrieve Employee from file with id 123456 to verify change
		ifs.open(binFile, istream::in | istream::binary);
		assert(ifs);
		unique_ptr<Employee>emp2(Employee::retrieve(ifs, id));
		cout << emp2->getSalary() << endl;
		ifs.close();
	}
	else cout << "Could not find Employee with id " << id << endl;

	//Step 12: Create new Employee to push into file
	id = 98765;
	salary = 200001;
	string name("Joseph Stoops"), address("123 Center St"), city("Orem"), state("Utah"), phone("555-555-5555");
	Employee newEmp(name, id, address, city, state, phone, salary);

	//Step 13: store this new Employee object into the file
	fs.open(binFile, fstream::in | fstream::out | fstream::binary);
	assert(fs);
	newEmp.store(fs);
	fs.flush();
	fs.close();

	//Step 14: Retrieve the same object from the file, and display it.
	ifs.open(binFile, istream::in | istream::binary);
	unique_ptr<Employee>emp3(Employee::retrieve(ifs, id));
	if (emp3 != nullptr) emp3->display(cout);
	else cout << "Could not find Employee with id " << id << endl;

	return 0;
}