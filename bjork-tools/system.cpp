#include "system.h"

bool RunSystemCommand(const char* kSystemCommand) {
    int res = system(kSystemCommand);
    return (res == 0); // res == 0 -> script ran correctly
}