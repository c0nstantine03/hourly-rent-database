#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// functions
void hello();
std::string read_pass();
int64_t find_user();
void sign_up();
std::string read_command();
void show_roominfo();


// 
class Role {
protected:
	int64_t id;
	std::string name;
public:
	Role();
	Role(int64_t, std::string);
	int64_t get_id() { return id; }
	std::string get_name() { return name; }
	void set_id(int64_t id) { this->id = id; }
	void set_name(std::string st) { name = st; }
	Role& operator=(Role&);
	void show();
};


// general action class
class User {
protected:
	int64_t id;
	std::string login;
	std::string password;
	std::string name;
	std::string phone;
	bool is_enable = 1;
	std::vector<Role> role;
	int active_role = 0; // 0 - guest, 1 - user, 2 - manager, 3 - admin
public:
	// base class methods
	User();
	User(int64_t, std::string, std::string, std::string, std::string);
	int64_t get_id() { return id; }
	std::string get_login() { return login; }
	std::string get_name() { return name; }
	std::string get_phone() { return phone; }
	bool get_enable() { return is_enable; }
	std::vector<Role>& get_role() { return role; }
	int get_active_role() { return active_role; }
	void set_id(int64_t id) { this->id = id; }
	void set_login(std::string st) { login = st; }
	void set_password(std::string st) { password = st; }
	void set_name(std::string st) { name = st; }
	void set_phone(std::string st) { phone = st; }
	void set_enable(bool t) { is_enable = t; }
	void set_role(std::vector<Role>& r) { role = r; }
	void set_active_role(int n) { active_role = n; }
	User& operator=(User&);

	// methods for all roles
	void show_userdata();
	void show_help();
	int64_t view_rooms();

	// methods for guest
	void sign_in();
	void sign_up();

	// methods for common user
	void old_bookings();
	void active_bookings();
	void book_room(int64_t);
	void my_comments();
	void write_comment(int64_t);
	void view_comments(int64_t);

	// methods for manager
	void edit_room(int64_t);
};


class Guest : public User {
public:
	Guest() : User() {};
};
/*
class ComUser : public User {
public:
	ComUser() : User() {};
	ComUser(int64_t id, std::string login, std::string password, std::string name, std::string phone) :
		User(id, login, password, name, phone) {};
	void show_commands();
	void book_room();
	void edit_booking();
	void delete_booking();
	void write_comment();
	void edit_comment();
	void delete_comment();
};

class Manager : public User {
public:
	Manager() : Customer() {};
	Manager(int64_t id, std::string login, std::string password, std::string name, std::string phone) :
		Customer(id, login, password, name, phone) {};
	void show_commands();
	void edit_roominfo();
	void recept_booking(); // accept-decline booking
};

class Admin : public User {
public:
	Admin() : Customer() {};
	Admin(int64_t id, std::string login, std::string password, std::string name, std::string phone) :
		Customer(id, login, password, name, phone) {};
	void show_commands();
	void add_room();
	void edit_roominfo();
	void delete_room();
	void view_users();
	void edit_users();
};
*/

// rooms that users booking
class Room {
protected:
	int64_t id;
	std::string name;
	std::string desc; // description
	std::string price; // price for 1 day
	std::string prepay; // prepayment
	int status = 0; // 0 - no bookings, 1 - have active bookings, 2 - room not available
public:
	Room();
	Room(int64_t, std::string, std::string, std::string, std::string);
	int64_t get_id() { return id; }
	std::string get_name() { return name; }
	std::string get_desc() { return desc; }
	std::string get_price() { return price; }
	std::string get_perc() { return prepay; }
	int get_status() { return status; }
	void set_id(int64_t id) { this->id = id; }
	void set_name(std::string st) { name = st; }
	void set_desc(std::string st) { desc = st; }
	void set_price(std::string st) { price = st; }
	void set_perc(std::string st) { prepay = st; }
	void set_status(int n) { status = n; }
	void show();
	Room& operator=(Room&);
};

// user creates booking, manager recepts it
class Booking {
protected:
	int64_t id;
	int64_t user_id;
	int64_t room_id;
	std::string date_begin;
	std::string date_end;
	int status = 0; // 0 - no recept, 1 - accept, 2 - decline, 3 - in process, 4 - closed
	std::string date_created = "";
	int64_t manager_id = 0;
public:
	Booking();
	Booking(int64_t, int64_t, int64_t, std::string, std::string);
	int64_t get_id() { return id; }
	int64_t get_userid() { return user_id; }
	int64_t get_roomid() { return room_id; }
	std::string get_begin() { return date_begin; }
	std::string get_end() { return date_end; }
	int get_status() { return status; }
	std::string get_created() { return date_created; }
	int64_t get_managerid() { return manager_id; }
	void set_id(int64_t id) { this->id = id; }
	void set_userid(int64_t id) { user_id = id; }
	void set_roomid(int64_t id) { room_id = id; }
	void set_begin(std::string dt) { date_begin = dt; }
	void set_end(std::string dt) { date_end = dt; }
	void set_status(int n) { status = n; }
	void set_created(std::string dt) { date_created = dt; }
	void set_managerid(int64_t id) { manager_id = id; }
	void show();
	Booking& operator=(Booking&);
};

// user may send feedback for closed bookings
class Comment {
protected:
	int64_t id;
	int64_t booking_id;
	int rating;
	std::string text;
	std::string date_created = "";
public:
	Comment();
	Comment(int64_t, int64_t, int, std::string);
	int64_t get_id() { return id; }
	int64_t get_bookingid() { return booking_id; }
	int get_rating() { return rating; }
	std::string get_text() { return text; }
	std::string get_created() { return date_created; }
	void set_id(int64_t id) { this->id = id; }
	void set_bookingid(int64_t id) { booking_id = id; }
	void set_rating(int n) { rating = n; }
	void set_text(std::string st) { text = st; }
	void set_created(std::string dt) { date_created = dt; }
	void show();
};


// friendly functions




