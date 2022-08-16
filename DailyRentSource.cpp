#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <regex>
#include <conio.h>
#include "DailyRent.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

extern sql::Connection* con;

//
// class Role
//

Role::Role()
{
	id = 1;	name = "User";
}

Role::Role(int64_t id, string nm)
{
	this->id = id;
	name = nm;
}

Role& Role::operator=(Role& r)
{
	id = r.id;
	name = r.name;
	return *this;
}

void Role::show()
{
	cout << "role id: " << setw(8) << id;
	cout << "  role name: " << name << endl;
}

//
// class User
//

User::User()
{
	id = 0; login = "Guest"; password = "p4ssw0rd"; name = ""; phone = "";
}

User::User(int64_t uid, string log, string pass, string nm, string ph)
{
	id = uid; login = log; password = pass; name = nm; phone = ph;
}

User& User::operator=(User& user)
{
	id = user.id;
	login = user.login;
	password = user.password;
	phone = user.phone;
	is_enable = user.is_enable;
	role = user.role;
	return *this;
}

void User::show_userdata()
{
	cout << "_____USER_INFO_____" << endl;
	cout << "login: " << login << endl;
	cout << "user name: " << name << endl;
	cout << "user phone: " << phone << endl << endl;
	for (int i = 0; i < role.size(); i++) {
		cout << "user role: " << role[i].get_name() << endl;
	}
	cout << endl;
}

void User::show_help()
{
	cout << "\n__________[MENU]__________\n";
	cout << "Commands:\n";
	cout << "  get help - '\\h'\n";
	cout << "  quit program - '\\q'\n";
	switch (active_role) {
	case 0:
		cout << "  sign in - '\\in'\n";
		cout << "  sign up - '\\up'\n";
		cout << "  view rooms - '\\vr'\n";
		break;
	case 1:
		cout << "  view rooms - '\\vr'\n";
		cout << "  view old bookings - '\\ob'\n";
		cout << "  view active bookings - '\\ab'\n";
		cout << "  view my comments - '\\mc'\n";
		cout << "  log out - '\\l'\n";
		break;
	case 2:
		cout << "  log out - '\\l'\n";
		break;
	case 3:
		cout << "  view rooms - '\\vr'\n";
		cout << "  log out - '\\l'\n";
		break;
	}
}

void User::sign_in()
{
	int64_t uid = find_user();
	if (uid == 0) {
		cout << "Wrong login or password\n";
		return;
	}
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res1, * res2;

	// select user data
	pstmt = con->prepareStatement("SELECT * FROM user_list WHERE user_id = ?");
	pstmt->setUInt64(1, uid);
	res1 = pstmt->executeQuery();
	delete pstmt;

	if (res1->getInt("is_enable")) {
		// select user roles
		pstmt = con->prepareStatement("SELECT * FROM user_role WHERE user_id = ?");
		pstmt->setUInt64(1, uid);
		res2 = pstmt->executeQuery();
		delete pstmt;

		while (res2->next()) {
			role.push_back(Role(res2->getUInt64(1), res2->getString(2)));
		}

		// set user data
		id = uid;
		login = res1->getString(2);
		password = res1->getString(3);
		name = res1->getString(4);
		phone = res1->getString(5);

		
		if (role.size() == 1) {
			active_role = role[0].get_id();
		}
		else {
			cout << "Your roles:\n";
			for (int i = 0; i < role.size(); i++) {
				role[i].show();
			}
			for (bool flag = 1; flag;) {
				int role_id;
				cout << "Chose role (id) >";
				cin >> role_id;
				for (int i = 0; i < role.size(); i++) {
					if (role[i].get_id() == role_id) {
						active_role = role_id;
						flag = 0;
						break;
					}
				}
			}
		}
		delete res2;
	}
	else {
		cout << "Oouuupsss! You're banned!\n";
		cout << "Please contact support\n";
	}
	delete res1; 
}

void User::sign_up()
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	while (true) {
		cout << "login >"; cin >> login;
		pstmt = con->prepareStatement("SELECT `user_id` FROM `user_list` WHERE `login` = ?");
		pstmt->setString(1, login);
		res = pstmt->executeQuery();
		if (res->next()) {
			cout << "this login already used by another user\n";
		}
		else {
			delete pstmt;
			delete res;
			break;
		}
	}
	while (true) {
		char c; string pass1;
		cout << "password >";
		while ((c = _getch()) != '\r')
		{
			password.push_back(c);
			_putch('*');
		}
		cout << "confirm password >";
		while ((c = _getch()) != '\r')
		{
			pass1.push_back(c);
			_putch('*');
		}
		if (password == pass1) {
			break;
		}
		else {
			cout << "wrong password\n";
		}
	}
	cout << "name"; 
	char c;
	while ((c = _getch()) != '\r')
	{
		name.push_back(c);
		_putch(c);
	}
	while (true)
	{
		cout << "phone in format XXX-XXX-XX-XX >"; cin >> phone;
		const regex r(R"(^([0-9]{3}-){2}[0-9]{2}-[0-9]{2}$)");
		if (regex_match(phone, r)) {
			break;
		}
		else {
			cout << "wrong phone format\n";
		}
	}
	pstmt = con->prepareStatement("CALL add_user(?, ?, ?, ?)");
	pstmt->setString(1, login);
	pstmt->setString(2, password);
	pstmt->setString(3, name);
	pstmt->setString(4, phone);
	pstmt->executeUpdate();
	delete pstmt;
	active_role = 1;
}

int64_t User::view_rooms()
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	int64_t rid = 0, count = 0;

	pstmt = con->prepareStatement("SELECT `room_id`, `name`, `description`, `std_price`, `std_price`*`prepayment_perc` AS prepay\
		FROM `rooms_list` WHERE `status` <> 2 AND `room_id` > ? ORDER BY `name` LIMIT 15");
	pstmt->setInt64(1, rid);
	res = pstmt->executeQuery();
	while (res->next() && count < 10) {
		rid = res->getInt64(1);
		Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
		room.show();
		count++;
	}
	string command;
	while (true) {
		cout << "\n__________MENU__________\n";
		if (res->next()) {
			cout << "  next - '\\n'\n";
			res->previous();
		}
		if (rid > count)
			cout << "  previous - '\\p'\n";
		cout << "  exit - '\\x'\n\n";
		switch (active_role) {
		case 1:
			cout << "  book room - '\\r'\n";
		case 0:
			cout << "  view comments - '\\vc'\n";
			break;
		case 2:
		case 3:
			cout << "  edit room - '\\r'\n";
			break;
		}
		
		cout << "write command >";
		command = read_command();
		if (command == "\\n") {
			if (res->next()) {
				pstmt->setInt64(1, rid);
				delete res; count = 0;
				res = pstmt->executeQuery();
				while (res->next() && count < 10) {
					rid = res->getInt64(1);
					Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
					room.show();
					count++;
				}
			}
		}
		else if (command == "\\p") {
			if (rid > count) {
				pstmt->setInt64(1, max(rid - count - 10, 0LL));
				delete res; count = 0;
				res = pstmt->executeQuery();
				while (res->next() && count < 10) {
					rid = res->getInt64(1);
					Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
					room.show();
					count++;
				}
			}
		}
		else if (command == "\\r" && active_role) {
			int64_t room_id = 0;
			while (true) {
				cout << "chose room (id) >";
				cin >> room_id;
				delete pstmt; delete res;
				pstmt = con->prepareStatement("SELECT room_id FROM rooms_list WHERE room_id = ?");
				pstmt->setInt64(1, room_id);
				res = pstmt->executeQuery();
				if (res->next()) {
					break;
				}
				else {
					cout << "this room is not exists\n";
				}
			}
			delete pstmt; delete res;
			return room_id;
		}
		else if (command == "\\vc" && active_role <= 1) {
			int64_t room_id = 0;
			while (true) {
				cout << "chose room (id) >";
				cin >> room_id;
				delete pstmt; delete res;
				pstmt = con->prepareStatement("SELECT room_id FROM rooms_list WHERE room_id = ?");
				pstmt->setInt64(1, room_id);
				res = pstmt->executeQuery();
				if (res->next()) {
					view_comments(room_id);
				}
				else {
					cout << "this room is not exists\n";
				}
			}
			delete pstmt; delete res;
			view_comments(room_id);
			return 0;
		}
		else if (command == "\\x") {
			delete pstmt; delete res;
			return 0;
		}
		else {
			cout << "Incorrect command\n";
		}
	}
}

void User::view_comments(int64_t id)
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	int64_t cid = 0, count = 0;

	pstmt = con->prepareStatement("SELECT * FROM comment_view WHERE `comment_id` > ? ORDER BY `rating` LIMIT 10");
	pstmt->setInt64(1, cid);
	res = pstmt->executeQuery();
	while (res->next() && count < 5) {
		cid = res->getInt64(1);
		Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
		room.show();
		count++;
	}
	string command;
	while (true) {
		cout << "\n__________MENU__________\n";
		if (res->next()) {
			cout << "  next - '\\n'\n";
			res->previous();
		}
		if (cid > count)
			cout << "  previous - '\\p'\n";
		cout << "  exit - '\\x'\n\n";

		cout << "write command >";
		command = read_command();
		if (command == "\\n") {
			if (res->next()) {
				pstmt->setInt64(1, cid);
				delete res; count = 0;
				res = pstmt->executeQuery();
				while (res->next() && count < 5) {
					cid = res->getInt64(1);
					Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
					room.show();
					count++;
				}
			}
		}
		else if (command == "\\p") {
			if (cid > count) {
				pstmt->setInt64(1, max(cid - count - 5, 0LL));
				delete res; count = 0;
				res = pstmt->executeQuery();
				while (res->next() && count < 10) {
					cid = res->getInt64(1);
					Room room(res->getInt64(1), res->getString(2), res->getString(3), res->getString(4), res->getString(5));
					room.show();
					count++;
				}
			}
		}
		else if (command == "\\x") {
			delete pstmt; delete res;
			return;
		}
		else {
			cout << "Incorrect command\n";
		}
	}
}

void User::book_room(int64_t room_id)
{
	cout << "__________MENU_________\n";
	cout << "  calendar bookings - '\\cb'\n";
	cout << "  create booking - '\\b'\n";
	cout << "  exit - '\\e'\n";

	string command;
	while (true) {
		cout << "write command >";
		command = read_command();

		if (command == "\\cb") {
			view_bookings_for(room_id);
		}
		else if (command == "\\b") {
			sql::PreparedStatement* pstmt;
			sql::ResultSet* res;
			string begin_, end_;
			cout << "date beginning YYYY-MM-DD >"; cin >> begin_;
			cout << "date end YYYY-MM-DD >"; cin >> end_;
			pstmt = con->prepareStatement("CALL add_booking(?, ?, ?, ?)");
			pstmt->setInt64(1, id);
			pstmt->setInt64(2, room_id);
			pstmt->setString(3, begin_);
			pstmt->setString(4, end_);
			res = pstmt->executeQuery();
			if (res->getInt(1)) {
				cout << "Booking created successfully\n";
			}
			else {
				cout << "Some days have already booked\n";
			}
			delete pstmt;
			delete res;
		}
	}
}

void User::old_bookings()
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	pstmt = con->prepareStatement("SELECT * FROM `history_booking_list` WHERE `user_id` = ? AND `status` = 4");
	pstmt->setInt64(1, id);
	res = pstmt->executeQuery();
	while (res->next()) {
		Booking book(res->getInt64(1), res->getInt64(2), res->getInt64(3), res->getString(4), res->getString(5));
		book.set_status(res->getInt(6));
		book.set_created(res->getString(7));
		book.set_managerid(res->getInt64(8));
		book.show();
	}
	delete pstmt; delete res;

	while (true) {
		cout << "__________MENU___________\n";
		cout << "  leave comment - '\\c'\n";
		cout << "  exit - '\\e'\n";

		string command;
		cout << "write command >";
		command = read_command();

		if (command == "\\c") {
			cout << "enter booking id. for exit - id = 0\n";
			int64_t id;
			cout << "booking id >";
			cin >> id;
			pstmt = con->prepareStatement("SELECT * FROM `history_booking_list` WHERE `booking_id` = ? AND `status` = 4");
			pstmt->setInt64(1, id);
			res = pstmt->executeQuery();
			if (res->next()) {
				if (id) {
					write_comment(id);
					break;
				}
				else {
					return;
				}
			}
			else {
				cout << "booking not found\n";
			}
			delete pstmt; delete res;
		}
		else if (command == "\\e") {
			return;
		}
	}
}

void User::active_bookings()
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	pstmt = con->prepareStatement("SELECT * FROM `booking_list` WHERE `user_id` = ?");
	pstmt->setInt64(1, id);
	res = pstmt->executeQuery();
	while (res->next()) {
		Booking book(res->getInt64(1), res->getInt64(2), res->getInt64(3), res->getString(4), res->getString(5));
		book.set_status(res->getInt(6));
		book.set_created(res->getString(7));
		book.set_managerid(res->getInt64(8));
		book.show();
	}
	delete pstmt; delete res;
}

//
// class Room
//

Room::Room()
{
	id = 0; name = ""; desc = ""; price = "0"; prepay = "0";
}

Room::Room(int64_t id, string name, string descr, string prc, string prep)
{
	this->id = id;
	this->name = name;
	desc = descr;
	price = prc;
	this->prepay = prep;
}

void Room::show()
{
	cout << "_____ROOM_INFO_____" << endl;
	cout << "Room: " << name << endl;
	cout << "Description: " << desc << endl;
	cout << "For 1 day:\tprice - " << price << "\tprepayment - " << prepay << endl << endl;
}

Room& Room::operator=(Room& r)
{
	id = r.id; name = r.name;
	desc = r.desc; price = r.price;
	prepay = r.prepay; status = r.status;
	return *this;
}

//
// class Booking
//

Booking::Booking()
{
	id = user_id = room_id = 0;
	date_begin = date_end = "";
}

Booking::Booking(int64_t bid, int64_t uid, int64_t rid, string dtb, string dte)
{
	id = bid; user_id = uid; room_id = rid;
	date_begin = dtb; date_end = dte;
}

void Booking::show()
{
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	cout << "_____BOOKING_____" << endl;
	cout << "id: " << id; 
	try 
	{
		pstmt = con->prepareStatement("SELECT `user_name` FROM `user_list` WHERE `user_id` = ?");
		pstmt->setInt64(1, user_id);
		res = pstmt->executeQuery();
		cout << "\tuser: " << res->getString(1);
		delete pstmt; delete res;

		pstmt = con->prepareStatement("SELECT `name` FROM `rooms_list` WHERE `room_id` = ?");
		pstmt->setInt64(1, room_id);
		res = pstmt->executeQuery();
		cout << "\troom: " << res->getString(1) << endl;
		delete pstmt; delete res;

		cout << "begin: " << date_begin;
		cout << "\tend: " << date_end << endl;
		cout << "status: ";
		switch (status) {
		case 0:
			cout << "not recept";
			break;
		case 1:
			cout << "accept";
			break;
		case 2:
			cout << "decline";
			break;
		case 3:
			cout << "in process";
			break;
		case 4:
			cout << "closed";
			break;
		}

		cout << "\nmanager: " << manager_id;
		cout << "\tcreated: " << date_created << endl << endl;
	}
	catch (sql::SQLException e)
	{
		cout << "Error: " << e.what() << endl;
	}
}

Booking& Booking::operator=(Booking& b)
{
	id = b.id; user_id = b.user_id; room_id = b.room_id;
	date_begin = b.date_begin; date_end = b.date_end;
	status = b.status; date_created = b.date_created;
	manager_id = b.manager_id;
	return *this;
}

//
// class Comment
//

Comment::Comment()
{
	id = 0; booking_id = 0; rating = 0; text = "";
}

Comment::Comment(int64_t cid, int64_t bid, int rat, string txt)
{
	id = cid; booking_id = bid; rating = rat; text = txt;
}

void Comment::show()
{

}

//
// functions
//


string read_command()
{
	string str;
	cin >> str;
	cin.clear();
	return str;
}

int64_t find_user()
{
	string login, password;
	cout << "write login >";
	cin >> login;
	cout << "write password >";
	char c;
	while ((c = _getch()) != '\r')
	{
		password.push_back(c);
		_putch('*');
	}

	int64_t user_id;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	pstmt = con->prepareStatement("SELECT coalesce(user_id, 0) FROM user_list WHERE `login` = ? AND `password` = ?");
	pstmt->setString(1, login);
	pstmt->setString(2, password);
	res = pstmt->executeQuery();
	user_id = res->getInt64(1);
	delete pstmt;
	delete res;
	return user_id;
}

void view_bookings_for(int64_t room_id)
{
	string begin_, end_;
	cout << "date beginning YYYY-MM-DD >"; cin >> begin_;
	cout << "date end YYYY-MM-DD >"; cin >> end_;

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	pstmt = con->prepareStatement("CALL calendar_bookings(?, ?, ?)");
	pstmt->setInt64(1, room_id);
	pstmt->setString(2, begin_);
	pstmt->setString(3, end_);
	res = pstmt->executeQuery();

	cout << "begin date\tend date\tstatus\n";
	while (res->next()) {
		cout << res->getString(1) << "\t";
		cout << res->getString(2) << "\t";
		cout << res->getString(3) << endl;
	}
	cout << endl;
	delete pstmt; delete res;
}





