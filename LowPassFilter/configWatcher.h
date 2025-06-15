#pragma once
#include "pch.h"
#include <windows.h>
#include <thread>
#include <chrono>
#include <functional>

// Define the callback type: takes a string (the config file path)
using ConfigChangedCallback = std::function<void()>;

class ConfigWatcher {    
public:
    ConfigWatcher(const std::wstring& directoryPath, const std::wstring& configFileName);
    ~ConfigWatcher();

    // Set the callback function
    void SetCallback(ConfigChangedCallback callback);

    // Start the monitoring thread
    bool Start();

    // Stop the monitoring thread
    void Stop();

private:
    std::wstring m_directoryPath;
    std::wstring m_configFileName;
    std::string m_fullConfigFilePath;
    std::atomic<bool> m_stopMonitoring;
    std::thread m_monitoringThread;
    HANDLE m_hDir;
    HANDLE m_hEvent;
    OVERLAPPED m_overlapped; // Member variable for overlapped structure

    ConfigChangedCallback m_callback;

    // Debouncing mechanism
    std::chrono::steady_clock::time_point m_lastConfigChangeTime = std::chrono::steady_clock::now();
    const std::chrono::milliseconds DEBOUNCE_INTERVAL = std::chrono::milliseconds(200); // 200 ms debounce

    void MonitorLoop();
};