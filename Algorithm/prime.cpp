#include <iostream>
#include <vector>

int countPrime(int n) {
    std::vector<bool> isPrime(n + 1, true);
    for(int i = 2; i * i <= n; i++) {
        if (isPrime[i]) {
            for(int j = i; j * i <= n; j++) {
                isPrime[j * i] = false;
            }
        }
    }
    int ans{};
    for(int i = 2; i <= n; i++) {
        if (isPrime[i]) {
            std::cout << i << '\n';
            ans++;
        }
    }
    return ans;
}

int main() {
    countPrime(97);
}
