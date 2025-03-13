#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include "DesktopUtils.h"

// Callback function for EnumWindows
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // Retrieve the vector of HWNDs passed as lParam
    std::vector<HWND>* workerwWindows = reinterpret_cast<std::vector<HWND>*>(lParam);

    // Get the class name of the window
    char className[256];
    if (GetClassNameA(hwnd, className, sizeof(className))) {
        // Check if the class name is "WorkerW"
        if (std::string(className) == "WorkerW") {
            workerwWindows->push_back(hwnd);
        }
    }
    return TRUE; // Continue enumeration
}

// Function to create and retrieve the WorkerW layer
extern "C" HWND CreateWorkerWLayer() {
    // Get the handle to the Progman window
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman) {
        return nullptr;
    }

    // Send a message to Progman to spawn a WorkerW window
    SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

    // Vector to store WorkerW windows
    std::vector<HWND> workerwWindows;

    // Enumerate all top-level windows
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&workerwWindows));

    // If we found WorkerW windows, return the last one
    if (!workerwWindows.empty()) {
        return workerwWindows.back();
    }

    return nullptr;
}

// Gets the location of the currently using wallpaper
wchar_t* GetCurrentWallpaper() {
    const wchar_t* regKey = L"Control Panel\\Desktop";
    const wchar_t* regValue = L"Wallpaper";
    HKEY hKey;

    // Open the registry key
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, regKey, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        throw std::runtime_error("Failed to open registry key, error code: " + std::to_string(result));
    }

    // Query the size of the value
    DWORD valueSize = 0;
    result = RegQueryValueExW(hKey, regValue, nullptr, nullptr, nullptr, &valueSize);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        throw std::runtime_error("Failed to query registry value size, error code: " + std::to_string(result));
    }

    // Allocate buffer for the value
    wchar_t* valueBuffer = new wchar_t[valueSize / sizeof(wchar_t)];
    result = RegQueryValueExW(hKey, regValue, nullptr, nullptr, reinterpret_cast<LPBYTE>(valueBuffer), &valueSize);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        delete[] valueBuffer; // Free allocated memory in case of failure
        throw std::runtime_error("Failed to query registry value, error code: " + std::to_string(result));
    }

    // Close the registry key
    RegCloseKey(hKey);

    return valueBuffer;
}
