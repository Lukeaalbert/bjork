#include "spinner.h"
#include <chrono>

std::atomic<bool> loading_done(false);

void LoadingSpinner() {
    const char spinner[] = {'|', '/', '-', '\\'};
    int index = 0;
    while (!loading_done) {
        std::cout << '\r' << spinner[index++ % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // clear spinner line
    std::cout << "\r  \n";
}
