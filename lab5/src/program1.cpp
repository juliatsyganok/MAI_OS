#include "contract.h"
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::cout << "Программа 1: реализация 1\n";
    std::cout << "Команды: 1 A B | 2 x | exit\n";
    std::string s;
    while (std::getline(std::cin, s)) {
        if (s == "exit") break;
        std::istringstream in(s);
        int cmd;
        if (!(in >> cmd)) {
            std::cout << "Некорректная команда\n";
            continue;
        }
        if (cmd == 1) {
            int a, b;
            std::string tmp;
            if (!(in >> a >> b) || (in >> tmp) || a < 1 || b < a) {
                std::cout << "Введите: 1 A B (1 <= A <= B)\n";
                continue;
            }
            std::cout << PrimeCount(a, b) << '\n';
        }
        else if (cmd == 2) {
            long x;
            std::string tmp;
            if (!(in >> x) || (in >> tmp)) {
                std::cout << "Введите: 2 x\n";
                continue;
            }
            std::cout << translation(x) << '\n';
        }
        else {
            std::cout << "Неизвестная команда\n";
        }
    }
}