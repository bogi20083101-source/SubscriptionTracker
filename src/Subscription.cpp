#include "Subscription.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

const std::string VALID_CATEGORIES[] = {"Развлечения", "Софт", "Обучение", "Коммунальные", "Другое"};
const std::string VALID_CYCLES[] = {"ежемесячно", "ежеквартально", "ежегодно"};
const std::string VALID_STATUSES[] = {"active", "paused", "cancelled"};

Subscription::Subscription() : monthlyCost(0.0) {}

Subscription::Subscription(const std::string &id, const std::string &serviceName, const std::string &category, double monthlyCost, const std::string &billingCycle, const std::string &nextPaymentDate, const std::string &status)
    : id(id), serviceName(serviceName), category(category),
      monthlyCost(monthlyCost), billingCycle(billingCycle),
      nextPaymentDate(nextPaymentDate), status(status) {}

std::string Subscription::getId() const { return id; }
std::string Subscription::getServiceName() const { return serviceName; }
std::string Subscription::getCategory() const { return category; }
double Subscription::getMonthlyCost() const { return monthlyCost; }
std::string Subscription::getBillingCycle() const { return billingCycle; }
std::string Subscription::getNextPaymentDate() const { return nextPaymentDate; }
std::string Subscription::getStatus() const { return status; }

void Subscription::setId(const std::string &newId) { id = newId; }
void Subscription::setServiceName(const std::string &name) { serviceName = name; }
void Subscription::setCategory(const std::string &cat) { category = cat; }
void Subscription::setMonthlyCost(double cost) { monthlyCost = cost; }
void Subscription::setBillingCycle(const std::string &cycle) { billingCycle = cycle; }
void Subscription::setNextPaymentDate(const std::string &date) { nextPaymentDate = date; }
void Subscription::setStatus(const std::string &newStatus) { status = newStatus; }

double Subscription::getEffectiveMonthlyCost() const {
    if (billingCycle == "ежеквартально") {
        return monthlyCost / 3.0;
    }
    else if (billingCycle == "ежегодно") {
        return monthlyCost / 12.0;
    }
    return monthlyCost;
}

std::string Subscription::toString() const {
    return id + "," + serviceName + "," + category + "," +
           std::to_string(monthlyCost) + "," + billingCycle + "," +
           nextPaymentDate + "," + status;
}

Subscription Subscription::fromString(const std::string &line) {
    std::istringstream iss(line);
    std::string id, serviceName, category, costStr, billingCycle, nextPaymentDate, status;

    if (!std::getline(iss, id, ',') ||
        !std::getline(iss, serviceName, ',') ||
        !std::getline(iss, category, ',') ||
        !std::getline(iss, costStr, ',') ||
        !std::getline(iss, billingCycle, ',') ||
        !std::getline(iss, nextPaymentDate, ',') ||
        !std::getline(iss, status, ',')) {
        throw std::runtime_error("Invalid line format: " + line);
    }

    double cost = std::stod(costStr);
    return Subscription(id, serviceName, category, cost, billingCycle, nextPaymentDate, status);
}

bool Subscription::validate(const Subscription &s) {
    if (s.id.empty() || s.serviceName.empty() || s.nextPaymentDate.empty()) return false;
    if (s.monthlyCost <= 0) return false;
    if (!isValidCategory(s.category)) return false;
    if (!isValidBillingCycle(s.billingCycle)) return false;
    if (!isValidStatus(s.status)) return false;
    if (!isValidDateFormat(s.nextPaymentDate)) return false;
    return true;
}

bool Subscription::isValidCategory(const std::string &category) {
    for (const auto &c : VALID_CATEGORIES) {
        if (c == category) return true;
    }
    return false;
}

bool Subscription::isValidBillingCycle(const std::string &cycle) {
    for (const auto &c : VALID_CYCLES) {
        if (c == cycle) return true;
    }
    return false;
}

bool Subscription::isValidStatus(const std::string &status) {
    for (const auto &s : VALID_STATUSES) {
        if (s == status) return true;
    }
    return false;
}

bool Subscription::isValidDateFormat(const std::string &date) {
    if (date.length() != 10) return false;
    if (date[2] != '.' || date[5] != '.') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(date[i])) return false;
    }
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (year < 1900 || year > 2100) return false;
    return true;
}