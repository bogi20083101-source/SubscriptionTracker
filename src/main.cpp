#include "Menu.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    SubscriptionManager manager;
    Menu menu(manager, "data/subscriptions.txt");
    menu.run();

    return 0;
}