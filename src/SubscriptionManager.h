#pragma once
#include "Subscription.h"
#include <functional>
#include <string>
#include <vector>

class SubscriptionManager {
private:
    std::vector<Subscription> subscriptions;

    static bool caseInsensitiveCompare(const std::string &a, const std::string &b);
    static int dateToInt(const std::string &date);

public:
    void add(const Subscription &sub);
    bool update(const std::string &id, const Subscription &updated);
    bool removeById(const std::string &id);
    bool removeByServiceName(const std::string &serviceName);

    Subscription *findById(const std::string &id);

    std::vector<Subscription> searchByServiceName(const std::string &name) const;
    std::vector<Subscription> searchByCategory(const std::string &category) const;
    std::vector<Subscription> searchByCostRange(double minCost, double maxCost) const;
    std::vector<Subscription> searchByDateRange(const std::string &minDate, const std::string &maxDate) const;

    void sortByMonthlyCost(bool ascending = true);
    void sortByNextPaymentDate(bool ascending = true);
    void sortByServiceName(bool ascending = true);

    double getTotalMonthlyCost() const;
    std::vector<Subscription> getUpcomingPayments(int days = 7) const;
    std::vector<std::pair<std::string, double>> getTopCategories(int count = 3) const;

    const std::vector<Subscription> &getAll() const;
    bool idExists(const std::string &id) const;
    void clear();
};