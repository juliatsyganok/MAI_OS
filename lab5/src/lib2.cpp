#include "contract.h"
#include <algorithm>
#include <string>
#include <vector>

extern "C" int PrimeCount(int A, int B) {
    if (A > B || B < 2) return 0;
    std::vector<bool> prime(static_cast<size_t>(B) + 1, true);
    prime[0] = false;
    prime[1] = false;
    for (long long i = 2; i * i <= B; ++i) {
        if (prime[i]) {
            for (long long j = i * i; j <= B; j += i) {
                prime[j] = false;
            }
        }
    }
    int cnt = 0;
    for (long long i = std::max(A, 2); i <= B; ++i) {
        if (prime[i]) {
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
        res.insert(res.begin(), '0' + n % 3);
        n /= 3;
    } while (n != 0);
    if (neg) {
        res.insert(res.begin(), '-');
    }
    return res.c_str();
}