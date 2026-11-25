#include "contract.h"
#include <dlfcn.h>
#include <iostream>
#include <sstream>
#include <string>

using PrimeFn = int (*)(int, int);
using TransFn = const char* (*)(long);

struct Library {
    void* h = nullptr;
    PrimeFn prime = nullptr;
    TransFn trans = nullptr;
    bool load(const char* path) {
        void* lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
        if (!lib) {
            std::cerr << "Ошибка загрузки: " << dlerror() << '\n';
            return false;
        }
        dlerror();
        auto p = reinterpret_cast<PrimeFn>(dlsym(lib, "PrimeCount"));
        const char* err = dlerror();
        if (err) {
            std::cerr << "Ошибка поиска функции: " << err << '\n';
            dlclose(lib);
            return false;
        }
        auto t = reinterpret_cast<TransFn>(dlsym(lib, "translation"));
        err = dlerror();
        if (err) {
            std::cerr << "Ошибка поиска функции: " << err << '\n';
            dlclose(lib);
            return false;
        }
        if (h) {
            dlclose(h);
        }
        h = lib;
        prime = p;
        trans = t;
        return true;
    }
    ~Library() {
        if (h) {
            dlclose(h);
        }
    }
};

int main() {
    Library lib;
    int cur = 1;
    if (!lib.load("./libimpl1.so")) {
        return 1;
    }
    std::cout << "Программа 2: реализация 1\n";
    std::cout << "Команды: 0 | 1 A B | 2 x | exit\n";
    std::string s;
    while (std::getline(std::cin, s)) {
        if (s == "exit") break;
        std::istringstream in(s);
        int cmd;
        if (!(in >> cmd)) {
            std::cout << "Некорректная команда\n";
            continue;
        }
        if (cmd == 0) {
            std::string tmp;
            if (in >> tmp) {
                std::cout << "Введите: 0\n";
                continue;
            }
            int next = cur == 1 ? 2 : 1;
            if (lib.load(next == 1 ? "./libimpl1.so" : "./libimpl2.so")) {
                cur = next;
                std::cout << "Реализация " << cur << '\n';
            }
        }
        else if (cmd == 1) {
            int a, b;
            std::string tmp;
            if (!(in >> a >> b) || (in >> tmp) || a < 1 || b < a) {
                std::cout << "Введите: 1 A B (1 <= A <= B)\n";
                continue;
            }
            std::cout << lib.prime(a, b) << '\n';
        }
        else if (cmd == 2) {
            long x;
            std::string tmp;
            if (!(in >> x) || (in >> tmp)) {
                std::cout << "Введите: 2 x\n";
                continue;
            }
            std::cout << lib.trans(x) << '\n';
        }
        else {
            std::cout << "Неизвестная команда\n";
        }
    }
}