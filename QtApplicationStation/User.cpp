#include "User.h"
#include "Source.h"

User::User() : name(""), surname(""), document(""), username(""), hashed_password(""), date_register(""), date_birth(""), role("") {}

std::string User::get_name() const { return name; }
std::string User::get_surname() const { return surname; }
std::string User::get_username() const { return username; }
std::string User::get_document() const { return document; }
std::string User::get_hashed_password() const { return hashed_password; }
std::string User::get_role() const { return role; };
std::string User::get_date_birth() const { return date_birth; };


Admin::Admin() : User() {};

void User::set_name(const std::string& nm) { name = nm; }
void User::set_surname(const std::string& nm) { surname = nm; }
void User::set_username(const std::string& u) { username = u; }
void User::set_document(const std::string& doc) { document = doc; }
void User::set_hashed_password(const std::string& psw) { hashed_password = psw; }
void User::set_date_register(const std::string& date) { date_register = date; }
void User::set_date_birth(const std::string& date) { date_birth = date; }
void User::set_role(const std::string& r) { role = r; };
