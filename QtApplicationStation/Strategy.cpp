#include "Strategy.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <fstream>


# define M_PI           3.14159265358979323846 

#define EARTH_RADIUS 6371.0

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
        std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return EARTH_RADIUS * c;
}

double StandardTicketPrice::calculatePrice(double distance, double time) const {
    double speed = distance / abs(time);
    std::cerr << speed << std::endl;


    if (speed < 50) {
        return distance * 3.0;
    }
    else if (speed < 100) {
        return distance * 5.0;
    }
    else if (speed < 125) {
        return distance * 6.0;
    }
    else {
        return distance * 7.0;
    }
}


double DiscountTicketPrice::calculatePrice(double distance, double time) const {
    double speed = distance / time;

    if (speed < 50) {
        return distance * 1.5;
    }
    else if (speed < 100) {
        return distance * 3.0;
    }
    else if (speed < 125) {
        return distance * 5.0;
    }
    else {
        return distance * 6.0;
    }
}
