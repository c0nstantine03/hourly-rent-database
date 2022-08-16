#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "DailyRent.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;


string Server = "tcp://127.0.0.1:3306";
string Username = "root";
sql::Connection* con;



int main()
{
	system("chcp 1251>null");
	// sql::Statement* stmt;
	// sql::PreparedStatement* pstmt;
	// sql::ResultSet* res;

	
	sql::Driver* driver;
	try
	{
		driver = get_driver_instance();
		con = driver->connect(Server, Username, read_pass());
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
	con->setSchema("dailyrentdb");
	
	string command = "";
	// enter like guest
	User* user = new User();

	hello();
	user->show_help();
	while (true) {
		// firstly we are guest
		cout << "write command >";
		command = read_command();

		// role = "Guest"
		if (user->get_active_role() == 0) {
			if (command == "\\in") {
				user->sign_in();
			}
			else if (command == "\\up") {
				user->sign_up();
			}
			else if (command == "\\vr") {
				user->view_rooms();
			}
			else if (command == "\\h") {
				user->show_help();
			}
			else if (command == "\\q") {
				break;
			}
			else {
				cout << "Incorrect command\n";
			}
		}
		// role = "User"
		else if (user->get_active_role() == 1) {
			if (command == "\\vr") {
				int64_t room_id = user->view_rooms();
				if (room_id) {
					user->book_room(room_id);
				}
			}
			if (command == "\\ob") {
				user->old_bookings();
			}
			if (command == "\\ab") {

			}
			if (command == "\\vc") {

			}
			else if (command == "\\h") {
				user->show_help();
			}
			else if (command == "\\l") {
				delete user;
				user = new User();
				cout << "Good by!\n";
			}
			else {
				cout << "Incorrect command\n";
			}
		}
		// role = "Manager"
		else if (user->get_active_role() == 2) {
			if (command == "\\vr") {
				int64_t room_id = user->view_rooms();
				if (room_id) {
					user->edit_room(room_id);
				}
			}
			else if (command == "\\h") {
				user->show_help();
			}
			else if (command == "\\l") {
				delete user;
				user = new User();
				cout << "Good by!\n";
			}
			else {
				cout << "Incorrect command\n";
			}
		}
		// role = "Admin"
		else if (user->get_active_role() == 3) {
			if (command == "\\vr") {
				int64_t room_id = user->view_rooms();
				if (room_id) {
					user->edit_room(room_id);
				}
			}
			else if (command == "\\h") {
				user->show_help();
			}
			else if (command == "\\l") {
				delete user;
				user = new User();
				cout << "Good by!\n";
			}
			else {
				cout << "Incorrect command\n";
			}
		}
	}

	delete con;
	if (user) delete user;
	system("pause");
	return EXIT_SUCCESS;
}

string read_pass()
{
	string filename("C:\\Users\\Kostya\\Downloads\\ЛАБЫ\\БД\\Курсовая_4_сем\\DailyRentDB\\sql_pass.txt");
	fstream inf(filename);
	string res = "";
	if (inf.is_open()) {
		inf >> res;
		inf.close();
	}
	else {
		cout << "File not found.";
	}
	return res;
}

void hello()
{
	cout << "\nWelcome to Daily Rent\n";
	cout << "Our contacts:\n";
	cout << "Phone: 080-555-3535\n";
	cout << "E-mail: post@mail.com\n";
	cout << endl;
}