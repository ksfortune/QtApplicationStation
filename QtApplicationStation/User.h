#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class User {
protected:
    std::string name;
    std::string surname;
    std::string document;
    std::string username;
    std::string hashed_password;
    std::string date_register;
    std::string date_birth;
    std::string role = "User";

public:
    User();

    std::string get_name() const;
    std::string get_surname() const;
    std::string get_username() const;
    std::string get_document() const;
    std::string get_hashed_password() const;
    std::string get_role() const;
    std::string get_date_birth() const;



    void set_role(const std::string& r);
    void set_name(const std::string& nm);
    void set_surname(const std::string& nm);
    void set_username(const std::string& u);
    void set_hashed_password(const std::string& psw);
    void set_document(const std::string& doc);
    void set_date_register(const std::string& date);
    void set_date_birth(const std::string& date);

};

class Admin : public User {
private:
    std::string role = "Admin";
public:
    Admin();
};
