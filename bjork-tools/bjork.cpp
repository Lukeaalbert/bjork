#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <string_view>

// lots to do here. for now just make sure we can access the error message.

void usage() {
    const static std::string usage = 
        "Usage:\n"
        "\tbjork-listen (capture new error message)\n"
        "\tbjork-listen --append (append error message onto previously captured error message)\n"
        "\tbjork-listen --show (show currently captured error message)\n"
        "\tbjork --explain (explain captured error message)\n"
        "\tbjork --set-explanation-complexity [1-10] (set complexity of explanations)\n"
        "\tbjork --help\n\n"
        "Sample Usage:"
        "\tbjork-listen g++ badcode.cpp\n"
        "\tbjork --explain\n";
}

int main(int argc, char *argv[]) {
    // invalid
    if (argc < 2) {
        usage();
        return 1;
    }

    std::string_view command(argv[1]);
    // invalid
    if (command.size() <= 2 || command[0] != '-' || command[1] != '-') {
        usage();
        return 1;
    }
    
    // remove --
    command.remove_prefix(2);

    // get path to last_error log (from bjork-listen)
    const char* path = std::getenv("HOME");
    if (!path) {
        std::cerr << "Could not find HOME environment variable.\n";
        return 1;
    }

    // open last_error log
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
