#include "Menu.h"
#include "FileIO.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

Menu::Menu(SubscriptionManager &manager, const std::string &filename)
    : manager(manager), filename(filename) {}

void Menu::printHeader() {
    std::cout << "\n==========================================\n";
    std::cout << "     Менеджер управления подписками       \n";
    std::cout << "==========================================\n";
    std::cout << "1. Добавить подписку\n";
    std::cout << "2. Редактировать подписку\n";
    std::cout << "3. Удалить подписку\n";
    std::cout << "4. Поиск и фильтрация\n";
    std::cout << "5. Сортировка подписок\n";
    std::cout << "6. Аналитика\n";
    std::cout << "7. Просмотр всех подписок\n";
    std::cout << "8. Сохранить данные\n";
    std::cout << "0. Выход\n";
    std::cout << "==========================================\n";
}

void Menu::printSubscription(const Subscription &s) {
    std::cout << "------------------------------------------\n";
    std::cout << "ID: " << s.getId() << "\n";
    std::cout << "Сервис: " << s.getServiceName() << "\n";
    std::cout << "Категория: " << s.getCategory() << "\n";
    std::cout << "Стоимость: " << std::fixed << std::setprecision(2) << s.getMonthlyCost() << "\n";
    std::cout << "Цикл оплаты: " << s.getBillingCycle() << "\n";
    std::cout << "Дата след. платежа: " << s.getNextPaymentDate() << "\n";
    std::cout << "Статус: " << s.getStatus() << "\n";
}

int Menu::getIntInput(const std::string &prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Неверный ввод. Попробуйте снова.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double Menu::getDoubleInput(const std::string &prompt, double min) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Неверный ввод. Попробуйте снова.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string Menu::getStringInput(const std::string &prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    while (value.empty()) {
        std::cout << "Поле не может быть пустым. Попробуйте снова.\n";
        std::cout << prompt;
        std::getline(std::cin, value);
    }
    return value;
}

void Menu::pressEnter() {
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Menu::addSubscription() {
    std::cout << "\n--- Добавление новой подписки ---\n";
    std::string id = getStringInput("ID: ");
    if (manager.idExists(id)) {
        std::cout << "Ошибка: Подписка с таким ID уже существует.\n";
        return;
    }
    std::string serviceName = getStringInput("Название сервиса: ");

    std::cout << "Категория (Развлечения, Софт, Обучение, Коммунальные, Другое): ";
    std::string category;
    std::getline(std::cin, category);
    while (!Subscription::isValidCategory(category)) {
        std::cout << "Неверная категория. Допустимо: Развлечения, Софт, Обучение, Коммунальные, Другое: ";
        std::getline(std::cin, category);
    }

    double monthlyCost = getDoubleInput("Стоимость за цикл: ", 0.01);

    std::cout << "Цикл оплаты (ежемесячно, ежеквартально, ежегодно): ";
    std::string billingCycle;
    std::getline(std::cin, billingCycle);
    while (!Subscription::isValidBillingCycle(billingCycle)) {
        std::cout << "Неверный цикл. Допустимо: ежемесячно, ежеквартально, ежегодно: ";
        std::getline(std::cin, billingCycle);
    }

    std::cout << "Дата следующего платежа (ДД.ММ.ГГГГ): ";
    std::string nextPaymentDate;
    std::getline(std::cin, nextPaymentDate);
    while (!Subscription::isValidDateFormat(nextPaymentDate)) {
        std::cout << "Неверный формат даты. Используйте ДД.ММ.ГГГГ: ";
        std::getline(std::cin, nextPaymentDate);
    }

    std::cout << "Статус (active, paused, cancelled): ";
    std::string status;
    std::getline(std::cin, status);
    while (!Subscription::isValidStatus(status)) {
        std::cout << "Неверный статус. Допустимо: active, paused, cancelled: ";
        std::getline(std::cin, status);
    }

    Subscription s(id, serviceName, category, monthlyCost, billingCycle, nextPaymentDate, status);
    if (!Subscription::validate(s)) {
        std::cout << "Ошибка: Неверные данные подписки.\n";
        return;
    }

    manager.add(s);
    std::cout << "Подписка успешно добавлена.\n";
}

void Menu::editSubscription() {
    std::cout << "\n--- Редактирование подписки ---\n";
    std::string id = getStringInput("Введите ID подписки для редактирования: ");
    Subscription *s = manager.findById(id);
    if (!s) {
        std::cout << "Подписка не найдена.\n";
        return;
    }

    printSubscription(*s);
    std::cout << "\nВведите новые данные (оставьте пустым, чтобы сохранить текущее значение):\n";

    std::cout << "ID [" << s->getId() << "]: ";
    std::string newId;
    std::getline(std::cin, newId);
    if (!newId.empty() && newId != s->getId()) {
        if (manager.idExists(newId)) {
            std::cout << "Ошибка: Подписка с таким ID уже существует.\n";
            return;
        }
        s->setId(newId);
    }

    std::cout << "Название сервиса [" << s->getServiceName() << "]: ";
    std::string newName;
    std::getline(std::cin, newName);
    if (!newName.empty()) s->setServiceName(newName);

    std::cout << "Категория [" << s->getCategory() << "]: ";
    std::string newCategory;
    std::getline(std::cin, newCategory);
    if (!newCategory.empty()) {
        if (Subscription::isValidCategory(newCategory)) {
            s->setCategory(newCategory);
        }
        else {
            std::cout << "Неверная категория. Оставлено текущее значение.\n";
        }
    }

    std::cout << "Стоимость за цикл [" << s->getMonthlyCost() << "]: ";
    std::string costStr;
    std::getline(std::cin, costStr);
    if (!costStr.empty()) {
        try {
            double cost = std::stod(costStr);
            if (cost > 0) s->setMonthlyCost(cost);
            else std::cout << "Стоимость должна быть > 0. Оставлено текущее значение.\n";
        }
        catch (...) {
            std::cout << "Неверная стоимость.\n";
        }
    }

    std::cout << "Цикл оплаты [" << s->getBillingCycle() << "]: ";
    std::string newCycle;
    std::getline(std::cin, newCycle);
    if (!newCycle.empty()) {
        if (Subscription::isValidBillingCycle(newCycle)) {
            s->setBillingCycle(newCycle);
        }
        else {
            std::cout << "Неверный цикл. Оставлено текущее значение.\n";
        }
    }

    std::cout << "Дата следующего платежа [" << s->getNextPaymentDate() << "]: ";
    std::string newDate;
    std::getline(std::cin, newDate);
    if (!newDate.empty()) {
        if (Subscription::isValidDateFormat(newDate)) {
            s->setNextPaymentDate(newDate);
        }
        else {
            std::cout << "Неверный формат даты. Оставлено текущее значение.\n";
        }
    }

    std::cout << "Статус [" << s->getStatus() << "]: ";
    std::string newStatus;
    std::getline(std::cin, newStatus);
    if (!newStatus.empty()) {
        if (Subscription::isValidStatus(newStatus)) {
            s->setStatus(newStatus);
        }
        else {
            std::cout << "Неверный статус. Оставлено текущее значение.\n";
        }
    }

    if (!Subscription::validate(*s)) {
        std::cout << "Ошибка: Неверные данные подписки.\n";
        return;
    }

    std::cout << "Подписка успешно обновлена.\n";
}

void Menu::deleteSubscription() {
    std::cout << "\n--- Удаление подписки ---\n";
    std::cout << "1. Удалить по ID\n";
    std::cout << "2. Удалить по названию сервиса\n";
    int choice = getIntInput("Выбор: ", 1, 2);

    if (choice == 1) {
        std::string id = getStringInput("Введите ID: ");
        if (manager.removeById(id)) {
            std::cout << "Подписка успешно удалена.\n";
        }
        else {
            std::cout << "Подписка не найдена.\n";
        }
    }
    else {
        std::string name = getStringInput("Введите название сервиса: ");
        if (manager.removeByServiceName(name)) {
            std::cout << "Подписка(и) успешно удалена(ы).\n";
        }
        else {
            std::cout << "Подписка не найдена.\n";
        }
    }
}

void Menu::searchSubscriptions() {
    std::cout << "\n--- Поиск и фильтрация ---\n";
    std::cout << "1. Поиск по названию сервиса (частичное совпадение)\n";
    std::cout << "2. Поиск по категории\n";
    std::cout << "3. Поиск по диапазону стоимости\n";
    std::cout << "4. Поиск по дате следующего платежа\n";
    int choice = getIntInput("Выбор: ", 1, 4);

    std::vector<Subscription> results;
    if (choice == 1) {
        std::string name = getStringInput("Введите название сервиса: ");
        results = manager.searchByServiceName(name);
    }
    else if (choice == 2) {
        std::cout << "Категория (Развлечения, Софт, Обучение, Коммунальные, Другое): ";
        std::string category;
        std::getline(std::cin, category);
        while (!Subscription::isValidCategory(category)) {
            std::cout << "Неверная категория: ";
            std::getline(std::cin, category);
        }
        results = manager.searchByCategory(category);
    }
    else if (choice == 3) {
        double minCost = getDoubleInput("Мин. стоимость: ", 0);
        double maxCost = getDoubleInput("Макс. стоимость: ", minCost);
        results = manager.searchByCostRange(minCost, maxCost);
    }
    else {
        std::string minDate = getStringInput("Мин. дата (ДД.ММ.ГГГГ): ");
        while (!Subscription::isValidDateFormat(minDate)) {
            std::cout << "Неверный формат даты. ";
            minDate = getStringInput("Мин. дата (ДД.ММ.ГГГГ): ");
        }
        std::string maxDate = getStringInput("Макс. дата (ДД.ММ.ГГГГ): ");
        while (!Subscription::isValidDateFormat(maxDate)) {
            std::cout << "Неверный формат даты. ";
            maxDate = getStringInput("Макс. дата (ДД.ММ.ГГГГ): ");
        }
        results = manager.searchByDateRange(minDate, maxDate);
    }

    if (results.empty()) {
        std::cout << "Подписки не найдены.\n";
    }
    else {
        std::cout << "\nНайдено подписок: " << results.size() << "\n";
        for (const auto &s : results) {
            printSubscription(s);
        }
    }
}

void Menu::sortSubscriptions() {
    std::cout << "\n--- Сортировка подписок ---\n";
    std::cout << "1. По месячной стоимости (по возрастанию)\n";
    std::cout << "2. По месячной стоимости (по убыванию)\n";
    std::cout << "3. По дате следующего платежа (по возрастанию)\n";
    std::cout << "4. По дате следующего платежа (по убыванию)\n";
    std::cout << "5. По названию сервиса (А-Я)\n";
    std::cout << "6. По названию сервиса (Я-А)\n";
    int choice = getIntInput("Выбор: ", 1, 6);

    switch (choice) {
        case 1:
            manager.sortByMonthlyCost(true);
            break;
        case 2:
            manager.sortByMonthlyCost(false);
            break;
        case 3:
            manager.sortByNextPaymentDate(true);
            break;
        case 4:
            manager.sortByNextPaymentDate(false);
            break;
        case 5:
            manager.sortByServiceName(true);
            break;
        case 6:
            manager.sortByServiceName(false);
            break;
    }
    std::cout << "Подписки отсортированы.\n";
}

void Menu::showAnalytics() {
    std::cout << "\n--- Аналитика ---\n";

    double totalMonthly = manager.getTotalMonthlyCost();
    std::cout << "Суммарные ежемесячные расходы: " << std::fixed << std::setprecision(2) << totalMonthly << "\n";

    auto upcoming = manager.getUpcomingPayments(7);
    std::cout << "\nПодписки с оплатой в ближайшие 7 дней: " << upcoming.size() << "\n";
    for (const auto &s : upcoming) {
        std::cout << "- " << s.getServiceName() << " (" << s.getNextPaymentDate() << ", "
                  << std::fixed << std::setprecision(2) << s.getMonthlyCost() << ")\n";
    }

    auto topCategories = manager.getTopCategories(3);
    std::cout << "\nТоп-3 категории по суммарной стоимости:\n";
    for (const auto &pair : topCategories) {
        std::cout << "- " << pair.first << ": " << std::fixed << std::setprecision(2) << pair.second << "\n";
    }
}

void Menu::saveData() {
    try {
        FileIO::save(manager.getAll(), filename);
        std::cout << "Данные успешно сохранены.\n";
    }
    catch (const std::exception &e) {
        std::cout << "Ошибка сохранения: " << e.what() << "\n";
    }
}

void Menu::run() {
    try {
        auto subscriptions = FileIO::load(filename);
        for (const auto &s : subscriptions) {
            manager.add(s);
        }
        std::cout << "Загружено подписок из файла: " << subscriptions.size() << "\n";
    }
    catch (const std::exception &e) {
        std::cout << "Примечание: " << e.what() << ". Запуск с пустыми данными.\n";
    }

    while (true) {
        printHeader();
        int choice = getIntInput("Выберите действие: ", 0, 8);

        switch (choice) {
            case 1:
                addSubscription();
                break;
            case 2:
                editSubscription();
                break;
            case 3:
                deleteSubscription();
                break;
            case 4:
                searchSubscriptions();
                break;
            case 5:
                sortSubscriptions();
                break;
            case 6:
                showAnalytics();
                break;
            case 7:
                std::cout << "\n--- Все подписки ---\n";
                for (const auto &s : manager.getAll()) {
                    printSubscription(s);
                }
                if (manager.getAll().empty()) {
                    std::cout << "Нет сохранённых подписок.\n";
                }
                break;
            case 8:
                saveData();
                break;
            case 0:
                std::cout << "Выход...\n";
                return;
        }
        pressEnter();
    }
}