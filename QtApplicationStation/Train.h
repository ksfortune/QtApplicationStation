#pragma once

#include <QString>
#include <QMap>
#include <string>
#include <QMetaType>


class Train {
protected:
    std::string train_name;
    std::string route;
    int wagons_count;
    int place_per_wag;
    QMap<QString, QString> schedule; 

public:
    Train();

    std::string get_name() const;
    std::string get_route() const;
    int get_wagons_count() const;
    int get_place_count() const;
    QMap<QString, QString> get_schedule() const;

    void set_new_name(const std::string& name);
    void set_new_route(const std::string& route);
    void set_new_wag_count(int wagons, int places);
    void set_places_per_wag(int places);

    void add_to_schedule(const QString& station, const QString& time);
};

