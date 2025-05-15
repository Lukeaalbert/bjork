// spinner.h
#pragma once
#include <atomic>
#include <iostream>
#include <thread>

extern std::atomic<bool> loading_done;

void LoadingSpinner();
