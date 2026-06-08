#pragma once
#include "SubscriptionManager.h"
#include <iostream>
#include <string>

class Menu {
private:
    SubscriptionManager &manager;
    std::string filename;

    void printHeader();
    void printSubscription(const Subscription &s);
    int getIntInput(const std::string &prompt, int min, int max);
    double getDoubleInput(const std::string &prompt, double min);
    std::string getStringInput(const std::string &prompt);
    void pressEnter();

    void addSubscription();
    void editSubscription();
    void deleteSubscription();
    void searchSubscriptions();
    void sortSubscriptions();
    void showAnalytics();
    void saveData();

public:
    Menu(SubscriptionManager &manager, const std::string &filename);
    void run();
};