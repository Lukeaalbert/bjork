#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <format>
#include <string_view>

#include <curl/curl.h>

namespace utils {
    struct ApiInfo {
        std::string_view kApiUri;
        std::string_view kQueryContent;
        std::string_view kSystemPrompt;
        std::string_view kJsonBody;
        std::string api_response;

        ApiInfo(std::string_view kApiUri_,
            std::string_view kQueryContent_,
            std::string_view kSystemPrompt_,
            std::string_view kJsonBody_):
            kApiUri(kApiUri_),
            kQueryContent(kQueryContent_),
            kSystemPrompt(kSystemPrompt_),
            kJsonBody(kJsonBody_),
            api_response(std::string())
            {}
    };

    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }
}

void Usage() {
    const static std::string usage = 
        "\nusage:\n"
        "\tbjork-listen (capture new error message)\n"
        "\n"
        "\tbjork --explain (explain captured error message)\n"
        "\tbjork --show (show currently captured error message)\n"
        "\tbjork --explanation-complexity [1-10] (set complexity of explanations)\n"
        "\tbjork --help\n\n"
        "sample usage:\n"
        "\tbjork-listen g++ badcode.cpp\n"
        "\tbjork --explain\n\n"
        "\tbjork-listen python3 badcode.py\n"
        "\tbjork --explain\n\n";
    std::cout << usage;
    std::cout.flush();
}

// populates api_info's api_response
void MakeApiCall(utils::ApiInfo* api_info) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    if (curl) {
        // set main stuff
        curl_easy_setopt(curl, CURLOPT_URL, api_info->kApiUri.data());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, api_info -> kJsonBody);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, api_info -> kJsonBody.size());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(api_info->api_response));

        // set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // preform curl!!!
        CURLcode response = curl_easy_perform(curl);

        if (response != CURLE_OK) {
            std::cerr << "Failed to query API for explanation!\n";
            exit(-1);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

void ExecuteRequest(std::string_view command, std::ifstream& file) {
    if (command == "show") {
        // print error
        std::cout << file.rdbuf();
        std::cout.flush();
    }
    else if (command == "explain") {
        // read file to string
        std::stringstream buff;
        buff << file.rdbuf();
        const std::string kLoggedError = buff.str();

        // make api call
        // TODO: big todo here! this is temporary. once we release,
        // don't just store this as a local env. will figure out more about
        // this later.
        const char* kGeminiApiKey = std::getenv("GEMINI_API_KEY");
        if (kGeminiApiKey == nullptr) {
            std::cerr << "Error: GEMINI_API_KEY environment variable not set." << std::endl;
            exit(-1);
        }

        std::ostringstream url_stream;
        url_stream << "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" << kGeminiApiKey;
        const std::string kApiUri = url_stream.str();

        // TODO: big todo here. make this prompt way better.
        const char* kSystemPrompt = 
        "You are a programmer. Your job is to explain the provided error to your student."
        "You will explain where the error is in their code, how to generally go about fixing it,"
        "and why it occured. Format your response with newlines and tabs (backslash n or t), as it"
        " will be directly displayed via std::cout.";

        std::ostringstream json_body_stream;
        json_body_stream << R"({
            "contents": [
                {
                    "role": "user",
                    "parts": [
                        { "text": ")" << kLoggedError << R"(" }
                    ]
                }
            ],
            "system_instruction": {
                "role": "system",
                "parts": [
                    { "text": ")" << kSystemPrompt << R"(" }
                ]
            }
        })";
        const std::string kJsonBody = json_body_stream.str();

        // make api info struct
        utils::ApiInfo api_info(kApiUri, kLoggedError, kSystemPrompt, kJsonBody);

        // make api call
        MakeApiCall(&api_info);

        // print api call
        std::cout << api_info.api_response << "\n";
    }
}

int main(int argc, char *argv[]) {

    // invalid (no flags set)
    if (argc < 2) {
        Usage();
        return 1;
    }

    std::string_view command(argv[1]);
    // invalid (bad flag syntax)
    if (command.size() <= 2 || command[0] != '-' || command[1] != '-') {
        Usage();
        return 1;
    }
    
    // remove --
    command.remove_prefix(2);

    // if command is --help, there's no need to open the file
    if (command == "help") {
        Usage();
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
        std::ifstream file(std::string(path) + "/.bjork/last_error");
        // invalid (last_error log is empty)
        if (!file) {
            std::cerr << "No error captured. Capture error message with 'bjork-listen' first.\n";
            return 1;
        }

        ExecuteRequest(command, file);

        file.close();
    }

    return 0;
}
