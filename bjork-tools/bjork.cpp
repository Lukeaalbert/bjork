#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <string_view>

void usage() {
    const static std::string usage = 
        "Usage:\n"
        "\tbjork-listen (capture new error message)\n"
        "\n"
        "\tbjork --explain (explain captured error message)\n"
        "\ttbjork --show (show currently captured error message)\n"
        "\tbjork --explanation-complexity [1-10] (set complexity of explanations)\n"
        "\tbjork --help\n\n"
        "Sample Usage:"
        "\tbjork-listen g++ badcode.cpp\n"
        "\tbjork --explain\n\n"
        "\tbjork-listen python3 badcode.py\n"
        "\tbjork --explain\n";
}

void executeRequest(std::string_view command, std::ifstream& file) {
    if (command == "show") {
        // print error
        std::string line{};
        std::stringstream buff;
        std::cout << file.rdbuf();
        std::cout.flush();
    }
    else if (command == "explain") {
        // read file to string
        std::stringstream buff;
        buff << file.rdbuf();
        std::string errorLog = buff.str();

        // make rpc call
    }
}

int main(int argc, char *argv[]) {

    // invalid (no flags set)
    if (argc < 2) {
        usage();
        return 1;
    }

    std::string_view command(argv[1]);
    // invalid (bad flag syntax)
    if (command.size() <= 2 || command[0] != '-' || command[1] != '-') {
        usage();
        return 1;
    }
    
    // remove --
    command.remove_prefix(2);

    // if command is --help, there's no need to open the file
    if (command == "help") {
        usage();
    } else if (command == "explanation-complexity") { // no need to open file here either
        // do stuff
    } else { // need to open file to execute user command
        // get path to last_error log (from bjork-listen)
        const char* path = std::getenv("HOME");

        // setup when wrong: invalid HOME env variable
        if (!path) {
            std::cerr << "Could not find HOME environment variable.\n";
            return 1;
        }

        // open last_error log
        std::ifstream file(std::string(path) + "/.last_error");
        // invalid (last_error log is empty)
        if (!file) {
            std::cerr << "No error captured. Capture error message with 'bjork-listen' first.\n";
            return 1;
        }

        executeRequest(command, file);

        file.close();
    }

    return 0;
}
