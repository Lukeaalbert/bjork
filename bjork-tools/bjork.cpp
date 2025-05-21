#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <format>
#include <optional>
#include <charconv>
#include <string_view>

#include "spinner.h"
#include "system.h"
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

    std::string EscapeJson(std::string_view input) {
        std::ostringstream oss;
        for (char c : input) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        oss << "\\u"
                            << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }

    std::string UnescapeJson(std::string_view input) {
        std::ostringstream out;
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '\\' && i + 1 < input.size()) {
                char next = input[i + 1];
                switch (next) {
                    case 'n': out << '\n'; i++; break;
                    case 't': out << '\t'; i++; break;
                    case 'r': out << '\r'; i++; break;
                    case 'b': out << '\b'; i++; break;
                    case 'f': out << '\f'; i++; break;
                    case '\\': out << '\\'; i++; break;
                    case '\"': out << '\"'; i++; break;
                    case 'u': {
                        if (i + 5 < input.size()) {
                            std::string code = std::string(input.substr(i + 2, 4));
                            char16_t unicode = static_cast<char16_t>(std::stoi(code, nullptr, 16));
                            if (unicode == 0x003c) out << '<';
                            else if (unicode == 0x003e) out << '>';
                            else out << '?';
                            i += 5;
                        }
                        break;
                    }
                    default: out << next; i++; break;
                }
            } else {
                out << input[i];
            }
        }
        return out.str();
    }


    std::optional<std::string> GetJsonStringValue(std::string_view json, std::string_view key) {
        std::string indentifier = "\"" + std::string(key) + "\"";

        size_t key_pos = json.find(indentifier);
        if (key_pos == std::string::npos)
            return std::nullopt;

        // find colon
        size_t colon_pos = json.find(':', key_pos + indentifier.size());
        if (colon_pos == std::string::npos)
            return std::nullopt;

        // skip any whitespace
        size_t quote_start = json.find('"', colon_pos);
        if (quote_start == std::string::npos)
            return std::nullopt;

        // find the closing quote while handling escaped quotes
        size_t i = quote_start + 1;
        std::string value;
        while (i < json.size()) {
            if (json[i] == '\\') {
                if (i + 1 < json.size()) {
                    value += json[i];
                    value += json[i + 1];
                    i += 2;
                } else {
                    // malformed
                    break;
                }
            } else if (json[i] == '"') {
                return value;
            } else {
                value += json[i++];
            }
        }
        // no closing quote
        return std::nullopt;
    }


    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }
}

void Usage() {
    const static std::string usage = 
        "usage:\n"
        "\tbjork --listen [compile/run command] (capture new error message)\n"
        "\tbjork --explain (explain captured error message)\n"
        "\tbjork --show (show currently captured error message and explanation complexity)\n"
        "\tbjork --explanation-complexity [1-10] (set complexity of explanations)\n"
        "\tbjork --help\n\n"
        "sample usage:\n"
        "\tbjork --listen g++ badcode.cpp\n"
        "\tbjork --explain\n\n"
        "\tbjork --listen python3 badcode.py\n"
        "\tbjork --explain\n";
    std::cout << usage;
    std::cout.flush();
}

// populates api_info's api_response
int MakeApiCall(utils::ApiInfo* api_info) {
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
            return 0;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return 1;
    }
    std::cerr << "Failed to query API for explanation!\n";
    return 0;
}

void ExecuteRequest(std::string_view command, const std::vector<std::string_view>& args) {
    if (command == "--help") { // no need to open files
        Usage();
    } else if (command == "--explanation-complexity") { // only need to open explanation-complexity file
        if (args.size() == 1) {
            // no int provided
            std::cerr << "Error: To set explanation complexity, please provide an int from 1-10 for complexity level.\n";
            exit(-1);
        }
        // get complexity from args
        int complexity = -1;
        auto [ptr, ec] = std::from_chars(args[1].data(), args[1].data() + args[1].size(), complexity);
        if (ec != std::errc() || complexity < 1 || complexity > 10) {
            // not an int
            std::cerr << "Error: To set explanation complexity, please provide an int from 1-10 for complexity level.\n";
            exit(-1);
        }

        // open file to write complexity to. clear prev content.
        std::string file_path = std::getenv("HOME") + std::string("/.bjork/bjork-explanation-complexity");
        std::ofstream explanation_complexity_file;
        explanation_complexity_file.open(file_path, std::ofstream::out | std::ofstream::trunc);

        // write complexity to file
        if (explanation_complexity_file.is_open()) {
            explanation_complexity_file << std::to_string(complexity);
            explanation_complexity_file.close();
            printf("Explanation complexity set to %d.\n", complexity);
        } else {
            std::cerr << "Could not open explanation complexity file at " << file_path << '\n';
            exit(-1);
        }
    } else if (command == "--listen") { // only need to write to last_error
        std::stringstream arg_stream;
        for (size_t i = 1; i < args.size(); ++i) arg_stream << args[i] << ' ';
        std::string home = std::getenv("HOME");
        std::string cmd = home + "/.local/share/bjork/bjork-listen " + arg_stream.str();
        bool res = RunSystemCommand(cmd.c_str());
        if (!res) {
            std::cerr << "Bjork --listen could not be run for " << arg_stream.str() << '\n';
            exit(-1);
        } else std::cout << '\n'; // for better formatting
    } else { // need to open both files
        // first check for invalid command
        if (command != "--show" && command != "--explain") {
            Usage(); // invalid command
            return;
        }
        // get path to files
        const char* path = std::getenv("HOME");

        // setup went wrong: invalid HOME env variable
        if (!path) {
            std::cerr << "Could not find HOME environment variable.\n";
            exit(-1);
        }

        // open last_error log
        std::ifstream log_file(std::string(path) + "/.bjork/last_error");
        if (!log_file) {
            std::cerr << "No error captured. Capture error message with 'bjork-listen' first.\n";
            exit(-1);
        }
        // open bjork-explanation-complexity log
        std::ifstream complexity_file(std::string(path) + "/.bjork/bjork-explanation-complexity");
        int complexity;
        if (!complexity_file) {
            std::cerr << "No explanation complexity found. Set with 'set-explanation-complexity' first.\n";
            exit(-1);
        }

        // start case for all commands that need to use files
        if (command == "--show") {
            // print error
            std::cout << log_file.rdbuf();
            // print explanation complexity
            std::cout << "\n*** Explanation Complexity:" << complexity_file.rdbuf() << " ***\n\n";
            std::cout.flush();
            // close files
            log_file.close();
            complexity_file.close();
        } else if (command == "--explain") {
            // this displays loading animation
            loading_done = false;
            std::thread spinner(LoadingSpinner);

            // read error log to string
            std::stringstream buff;
            buff << log_file.rdbuf();
            const std::string kLoggedError =  buff.str();

            // read explanation complexity to string
            std::stringstream buff_;
            buff_ << complexity_file.rdbuf();
            const std::string kExplanationComplexity =  buff_.str();

            // make api call
            // TODO: big todo here! this is temporary. once we release,
            // don't just store this as a local env. will figure out more about
            // this later.
            const char* kGeminiApiKey = std::getenv("GEMINI_API_KEY");
            if (kGeminiApiKey == nullptr) {
                std::cerr << "Error: GEMINI_API_KEY environment variable not set." << std::endl;
                loading_done = true;
                spinner.join(); // always
                log_file.close();
                complexity_file.close();
                exit(-1);
            }

            std::ostringstream url_stream;
            url_stream << "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" << kGeminiApiKey;
            const std::string kApiUri = url_stream.str();

            // TODO: improve as much as possible
            const char* kSystemPrompt = R"(
            You are a programming tutor assistant. You will be shown an error message.

            Your job is to generate a brief, clear, and helpful explanation of the error.

            Your response MUST contain exactly four sections, with these exact labels:

            The Error: <Short summary of what the error is>

            Likely Location: <Where in the user's code the problem likely is (e.g. line number, header, or code context)>

            How to Fix: <How the user can fix or approach fixing the error>

            Why It Happened: <One-sentence explanation of why this error occurs in general>

            RESPONSE FORMAT REQUIREMENTS:
            - Use *exactly* the labels: "The Error:", "Likely Location:", "How to Fix:", and "Why It Happened:"
            - Do NOT use markdown, code blocks, bullet points, or any extra formatting
            - The output must be suitable for printing directly via std::cout
            - Each section must be separated by **exactly two real newline characters** (not escaped `\\n`, not double-escaped)
            - There must be **no additional newlines** anywhere else — not at the beginning, not at the end, not between label and content
            - Output only the four labeled sections — no introductory or summary text

            )";

            std::ostringstream json_body_stream;
            json_body_stream << R"({
                "contents": [
                    {
                        "role": "user",
                        "parts": [
                            { "text": ")" << utils::EscapeJson(kLoggedError) << R"(" }
                        ]
                    }
                ],
                "system_instruction": {
                    "role": "system",
                    "parts": [
                        { "text": ")" << utils::EscapeJson(kSystemPrompt) << R"(" }
                    ]
                }
            })";
            const std::string kJsonBody = json_body_stream.str();

            // make api info struct
            utils::ApiInfo api_info(kApiUri, kLoggedError, kSystemPrompt, kJsonBody);

            // make api call
            int res_status = MakeApiCall(&api_info);

            // api call done!
            loading_done = true;
            spinner.join();

            if (!res_status) {
                log_file.close();
                complexity_file.close();
                exit(-1);
            }

            // parse code and message; print
            auto explanation = utils::GetJsonStringValue(api_info.api_response, "text");
            if (!explanation) {
                std::cerr << "Error: No explanation text found in response.\n";
                std::cerr << "Full API response:\n" << api_info.api_response << '\n';
                log_file.close();
                complexity_file.close();
                exit(-1);
            } else {
                std::cout << utils::UnescapeJson(*explanation) << '\n';
            }
        }
        log_file.close();
        complexity_file.close();
    }
}

int main(int argc, char *argv[]) {
    // invalid (no flags set)
    if (argc < 2) {
        Usage();
        return 1;
    }

    // holds all args
    std::vector<std::string_view> args;
    for (size_t i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    std::string_view command = (argv[1]);
    // invalid (bad flag syntax)
    if (command.size() <= 2 || command[0] != '-' || command[1] != '-') {
        Usage();
        return 1;
    }

    ExecuteRequest(command, args);

    return 0;
}
