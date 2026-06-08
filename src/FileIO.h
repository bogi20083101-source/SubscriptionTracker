#pragma once
#include "Subscription.h"
#include <string>
#include <vector>

class FileIO {
public:
    static void save(const std::vector<Subscription> &subscriptions, const std::string &filename);
    static std::vector<Subscription> load(const std::string &filename);
};