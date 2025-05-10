#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// lots to do here. for now just make sure we can access the error message.

int main() {
    const char* path = std::getenv("HOME");
    if (!path) {
        std::cerr << "Could not find HOME environment variable.\n";
        return 1;
    }

    std::ifstream file(std::string(path) + "/.last_error");
    if (!file) {
        std::cerr << "No error captured. Run something with 'myrun' first.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << '\n';
    }

    return 0;
}
