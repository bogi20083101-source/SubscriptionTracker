#include "SubscriptionManager.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <map>
#include <stdexcept>

bool SubscriptionManager::caseInsensitiveCompare(const std::string &a, const std::string &b) {
    std::string ca = a, cb = b;
    std::transform(ca.begin(), ca.end(), ca.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(cb.begin(), cb.end(), cb.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ca < cb;
}

int SubscriptionManager::dateToInt(const std::string &date) {
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));
    return year * 10000 + month * 100 + day;
}

static std::string getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm buf;
#if defined(_WIN32)
    localtime_s(&buf, &t);
#else
    localtime_r(&t, &buf);
#endif
    char out[11];
    std::snprintf(out, sizeof(out), "%02d.%02d.%04d", buf.tm_mday, buf.tm_mon + 1, buf.tm_year + 1900);
    return std::string(out);
}

void SubscriptionManager::add(const Subscription &sub) {
    if (idExists(sub.getId())) {
        throw std::runtime_error("Subscription with ID " + sub.getId() + " already exists");
    }
    subscriptions.push_back(sub);
}

bool SubscriptionManager::update(const std::string &id, const Subscription &updated) {
    for (auto &s : subscriptions) {
        if (s.getId() == id) {
            if (updated.getId() != id && idExists(updated.getId())) {
                return false;
            }
            s = updated;
            return true;
        }
    }
    return false;
}

bool SubscriptionManager::removeById(const std::string &id) {
    auto it = std::remove_if(subscriptions.begin(), subscriptions.end(), [&id](const Subscription &s) { return s.getId() == id; });
    if (it != subscriptions.end()) {
        subscriptions.erase(it, subscriptions.end());
        return true;
    }
    return false;
}

bool SubscriptionManager::removeByServiceName(const std::string &serviceName) {
    std::string searchLower = serviceName;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    auto it = std::remove_if(subscriptions.begin(), subscriptions.end(), [&searchLower](const Subscription &s) {
        std::string sn = s.getServiceName();
        std::transform(sn.begin(), sn.end(), sn.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return sn == searchLower;
    });
    if (it != subscriptions.end()) {
        subscriptions.erase(it, subscriptions.end());
        return true;
    }
    return false;
}

Subscription *SubscriptionManager::findById(const std::string &id) {
    for (auto &s : subscriptions) {
        if (s.getId() == id) return &s;
    }
    return nullptr;
}

std::vector<Subscription> SubscriptionManager::searchByServiceName(const std::string &name) const {
    std::string searchLower = name;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    std::vector<Subscription> result;
    for (const auto &s : subscriptions) {
        std::string sn = s.getServiceName();
        std::transform(sn.begin(), sn.end(), sn.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (sn.find(searchLower) != std::string::npos) {
            result.push_back(s);
        }
    }
    return result;
}

std::vector<Subscription> SubscriptionManager::searchByCategory(const std::string &category) const {
    std::vector<Subscription> result;
    for (const auto &s : subscriptions) {
        if (s.getCategory() == category) {
            result.push_back(s);
        }
    }
    return result;
}

std::vector<Subscription> SubscriptionManager::searchByCostRange(double minCost, double maxCost) const {
    std::vector<Subscription> result;
    for (const auto &s : subscriptions) {
        if (s.getMonthlyCost() >= minCost && s.getMonthlyCost() <= maxCost) {
            result.push_back(s);
        }
    }
    return result;
}

std::vector<Subscription> SubscriptionManager::searchByDateRange(const std::string &minDate, const std::string &maxDate) const {
    int minVal = dateToInt(minDate);
    int maxVal = dateToInt(maxDate);
    std::vector<Subscription> result;
    for (const auto &s : subscriptions) {
        int d = dateToInt(s.getNextPaymentDate());
        if (d >= minVal && d <= maxVal) {
            result.push_back(s);
        }
    }
    return result;
}

void SubscriptionManager::sortByMonthlyCost(bool ascending) {
    std::sort(subscriptions.begin(), subscriptions.end(), [ascending](const Subscription &a, const Subscription &b) {
        return ascending ? a.getMonthlyCost() < b.getMonthlyCost()
                         : a.getMonthlyCost() > b.getMonthlyCost();
    });
}

void SubscriptionManager::sortByNextPaymentDate(bool ascending) {
    std::sort(subscriptions.begin(), subscriptions.end(), [ascending](const Subscription &a, const Subscription &b) {
        int da = dateToInt(a.getNextPaymentDate());
        int db = dateToInt(b.getNextPaymentDate());
        return ascending ? da < db : da > db;
    });
}

void SubscriptionManager::sortByServiceName(bool ascending) {
    std::sort(subscriptions.begin(), subscriptions.end(), [ascending](const Subscription &a, const Subscription &b) {
        bool result = caseInsensitiveCompare(a.getServiceName(), b.getServiceName());
        return ascending ? result : !result;
    });
}

double SubscriptionManager::getTotalMonthlyCost() const {
    double total = 0.0;
    for (const auto &s : subscriptions) {
        if (s.getStatus() == "active") {
            total += s.getEffectiveMonthlyCost();
        }
    }
    return total;
}

std::vector<Subscription> SubscriptionManager::getUpcomingPayments(int days) const {
    std::string todayStr = getCurrentDateString();
    int todayInt = dateToInt(todayStr);
    int futureInt = todayInt + days;

    std::vector<Subscription> result;
    for (const auto &s : subscriptions) {
        if (s.getStatus() == "active") {
            int d = dateToInt(s.getNextPaymentDate());
            if (d >= todayInt && d <= futureInt) {
                result.push_back(s);
            }
        }
    }
    return result;
}

std::vector<std::pair<std::string, double>> SubscriptionManager::getTopCategories(int count) const {
    std::map<std::string, double> categoryTotals;
    for (const auto &s : subscriptions) {
        if (s.getStatus() == "active") {
            categoryTotals[s.getCategory()] += s.getEffectiveMonthlyCost();
        }
    }

    std::vector<std::pair<std::string, double>> sorted;
    for (const auto &pair : categoryTotals) {
        sorted.push_back(pair);
    }
    std::sort(sorted.begin(), sorted.end(), [](const std::pair<std::string, double> &a, const std::pair<std::string, double> &b) {
        return a.second > b.second;
    });

    if ((int)sorted.size() > count) {
        sorted.resize(count);
    }
    return sorted;
}

const std::vector<Subscription> &SubscriptionManager::getAll() const {
    return subscriptions;
}

bool SubscriptionManager::idExists(const std::string &id) const {
    for (const auto &s : subscriptions) {
        if (s.getId() == id) return true;
    }
    return false;
}

void SubscriptionManager::clear() {
    subscriptions.clear();
}