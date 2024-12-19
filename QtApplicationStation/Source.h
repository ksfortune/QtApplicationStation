#pragma once
#include <iostream>
#include <memory>
#include <QTextStream>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include <QComboBox>
#include <QDateEdit>
#include "User.h"
#include "Train.h"
#include "sqlite3.h"
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QMap>
#include <QPair>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>


struct Date {
    int day;
    int month;
    int year;
};

struct Time {
    int hour;
    int minute;
};

struct Station {
    std::string train_name;
    double latitude;
    double longitude;
    Date date;
    Time time;
    std::string station_name;
};

QStringList loadStationsFromFile(const QString& fileName);
QString getCoordinatesForStation(const QString& stationName);
QStringList getListOfTrains();
QMap<QString, std::vector<QPair<QString, QString>>> getStationsData();
int time_between_to_times_(const QString& date1, const QString& date2, int type);
std::vector<QPair<QString, QString>> checkTrainAtStations(const QString& station1, const QString& station2, const QString& date, const QMap<QString, std::vector<QPair<QString, QString>>>& stationsMap);
void writeStationsToFile(const QList<QVariantMap>& stations);
Train* createTrainByName(const QString& name);
Train* get_train_from_db(const std::string& train_name);
void setScheduleToTrain(Train* train);
std::vector<std::pair<int, std::string>> getSeatsInfo(const std::string& trainName, int wagonNumber);
User* GetUserByData_(const QString& usr);
int CheckIfAdmin_(const QString& usr, const QString& psw);
int FindUserData_(const QString& usr, const QString& psw);
unsigned int DJBHash_(const QString& str);
void occupy_place(QString train_name, int wagonNumber, int seatNumber, User* user);
int appendUserData_(const QString& name, const QString& surname, const QString& bd, const QString& usr, const QString& psw);
int saveTrainData_(const QString& trainName, const QString& route, int wagonsCount, int placePerWag, const QStringList& stationsList);
QStringList loadTrainsFromFile();