#include "contract.h"
#include <string>

extern "C" int PrimeCount(int A, int B) {
    if (A > B) return 0;
    int cnt = 0;
    for (long long i = (A < 2 ? 2 : A); i <= B; ++i) {
        bool ok = true;
        for (long long j = 2; j < i; ++j) {
            if (i % j == 0) {
                ok = false;
                break;
            }
        }
        if (ok) {
            ++cnt;
        }
    }
    return cnt;
}

extern "C" const char* translation(long x) {
    static thread_local std::string res;
    bool neg = x < 0;
    unsigned long n = static_cast<unsigned long>(x);
    if (neg) {
        n = 0UL - n;
    }
    res.clear();
    do {
        res.insert(res.begin(), '0' + n % 2);
        n /= 2;
    } while (n != 0);
    if (neg) {
        res.insert(res.begin(), '-');
    }
    return res.c_str();
}