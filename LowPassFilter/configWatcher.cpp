#include "pch.h"
#include "configWatcher.h"
#include <string>
#include <filesystem>

ConfigWatcher::ConfigWatcher(const std::wstring& directoryPath, const std::wstring& configFileName)
    : m_directoryPath(directoryPath),
    m_configFileName(configFileName),
    m_stopMonitoring(false),
    m_hDir(INVALID_HANDLE_VALUE),
    m_hEvent(NULL)
{
}

ConfigWatcher::~ConfigWatcher() {
    Stop();
    if (m_hEvent != NULL) {
        CloseHandle(m_hEvent);
    }
    if (m_hDir != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDir);
    }
}

// Set the callback function
void ConfigWatcher::SetCallback(ConfigChangedCallback callback) {
    m_callback = callback;
}

// Start the monitoring thread
bool ConfigWatcher::Start() {
    if (m_monitoringThread.joinable()) {
        return false;
    }

    m_hDir = CreateFileW(
        m_directoryPath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (m_hDir == INVALID_HANDLE_VALUE) {
        return false;
    }

    m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event
    if (m_hEvent == NULL) {
        CloseHandle(m_hDir);
        m_hDir = INVALID_HANDLE_VALUE; // Mark as invalid
        return false;
    }

    m_stopMonitoring.store(false);
    m_monitoringThread = std::thread(&ConfigWatcher::MonitorLoop, this);
    return true;
}

// Stop the monitoring thread
void ConfigWatcher::Stop() {
    if (m_monitoringThread.joinable()) {
        m_stopMonitoring.store(true); // Signal thread to stop
        SetEvent(m_hEvent); // Unblock ReadDirectoryChangesW if it's waiting
        m_monitoringThread.join(); // Wait for the thread to finish
    }
}

void ConfigWatcher::MonitorLoop() {
    BYTE buffer[4096]; // Buffer for change notifications
    DWORD bytesReturned;

    m_overlapped.hEvent = m_hEvent; // Link event to overlapped structure

    while (!m_stopMonitoring.load()) {
        ResetEvent(m_hEvent); // Reset event before each ReadDirectoryChangesW call

        BOOL success = ReadDirectoryChangesW(
            m_hDir,
            buffer,
            sizeof(buffer),
            FALSE, // Monitor subdirectories (FALSE for only the target directory)
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE,
            &bytesReturned,
            &m_overlapped,
            NULL
        );

        if (!success && GetLastError() != ERROR_IO_PENDING) {
            break;
        }

        // Wait for the event to be signaled or for stop
        HANDLE waitHandles[] = { m_hEvent };
        DWORD waitResult = WaitForMultipleObjects(1, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) { // Event was signaled
            if (m_stopMonitoring.load()) { // Check stop flag immediately after waking
                break; // Exit if stop was requested
            }

            if (!GetOverlappedResult(m_hDir, &m_overlapped, &bytesReturned, FALSE)) {
                continue;
            }

            if (bytesReturned == 0) {
                // This can happen if the buffer was too small or a non-relevant event occurred.
                // Just continue the loop to re-issue the request.
                continue;
            }

            FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
            while (pNotify) {
                std::wstring fileNameW(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                std::string fileNameA(fileNameW.begin(), fileNameW.end()); // Convert to narrow string for comparison/callback

 
                if (fileNameA == std::filesystem::path(m_configFileName).string()) { // Compare narrow string
                    auto currentTime = std::chrono::steady_clock::now();
                    if (currentTime - m_lastConfigChangeTime > DEBOUNCE_INTERVAL) {
                        m_lastConfigChangeTime = currentTime;
                        if (m_callback) { // Check if a callback is set
                            m_callback();
                        }
                    }
                }

                if (pNotify->NextEntryOffset == 0) {
                    break; // No more entries in this buffer
                }
                pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset);
            }
        }
        else if (waitResult == WAIT_FAILED) {
            break;
        }
    }
}