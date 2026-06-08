#pragma once
#include <sstream>
#include <string>

class Subscription {
private:
    std::string id;
    std::string serviceName;
    std::string category;
    double monthlyCost;
    std::string billingCycle;
    std::string nextPaymentDate;
    std::string status;

public:
    Subscription();
    Subscription(const std::string &id, const std::string &serviceName, const std::string &category, double monthlyCost, const std::string &billingCycle, const std::string &nextPaymentDate, const std::string &status);

    std::string getId() const;
    std::string getServiceName() const;
    std::string getCategory() const;
    double getMonthlyCost() const;
    std::string getBillingCycle() const;
    std::string getNextPaymentDate() const;
    std::string getStatus() const;

    void setId(const std::string &id);
    void setServiceName(const std::string &name);
    void setCategory(const std::string &category);
    void setMonthlyCost(double cost);
    void setBillingCycle(const std::string &cycle);
    void setNextPaymentDate(const std::string &date);
    void setStatus(const std::string &status);

    double getEffectiveMonthlyCost() const;

    std::string toString() const;
    static Subscription fromString(const std::string &line);
    static bool validate(const Subscription &s);
    static bool isValidCategory(const std::string &category);
    static bool isValidBillingCycle(const std::string &cycle);
    static bool isValidStatus(const std::string &status);
    static bool isValidDateFormat(const std::string &date);
};