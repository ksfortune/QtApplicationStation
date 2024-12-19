#include "Train.h"

Train::Train() : train_name(""), route(""), wagons_count(0), place_per_wag(0), schedule() {}

std::string Train::get_name() const {
    return train_name;
}

std::string Train::get_route() const {
    return route;
}

int Train::get_wagons_count() const {
    return wagons_count;
}

int Train::get_place_count() const {
    return place_per_wag;
}

QMap<QString, QString> Train::get_schedule() const {
    return schedule;
}

void Train::set_new_name(const std::string& name) {
    train_name = name;
}

void Train::set_new_route(const std::string& route) {
    this->route = route;
}

void Train::set_new_wag_count(int wagons, int places) {
    wagons_count = wagons;
    place_per_wag = places;
}

void Train::set_places_per_wag(int places) {
    place_per_wag = places;
}

void Train::add_to_schedule(const QString& station, const QString& time) {
    schedule[station] = time;  
}
