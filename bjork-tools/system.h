// bash.h
#pragma once
#include <cstdlib> // runs bash scripts for us

// runs script for us, return value indicates success or not 
bool RunSystemCommand(const char* kSystemCommand);
