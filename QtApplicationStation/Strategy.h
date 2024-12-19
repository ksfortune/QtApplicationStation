#pragma once
class TicketPriceStrategy {
public:
    virtual double calculatePrice(double distance, double time) const = 0;
    virtual ~TicketPriceStrategy() = default;
};

class StandardTicketPrice : public TicketPriceStrategy {
public:
    double calculatePrice(double distance, double time) const override;
};

class DiscountTicketPrice : public TicketPriceStrategy {
public:
    double calculatePrice(double distance, double time) const override;
};

double calculateDistance(double lat1, double lon1, double lat2, double lon2);
