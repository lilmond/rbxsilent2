#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

// Structure to pass data to our window enumeration callbacks
struct TargetData {
    DWORD targetPid;
    bool found;
};

// Callback to find a Roblox window matching a specific PID and change its visibility
static BOOL CALLBACK SetVisibilityByPidCallback(HWND hWnd, LPARAM lParam) {
    TargetData* data = reinterpret_cast<TargetData*>(lParam);
    
    int length = GetWindowTextLength(hWnd) + 1;
    char* buffer = new char[length];
    GetWindowText(hWnd, buffer, length);
    string title(buffer);
    delete[] buffer;

    if (title == "Roblox") {
        DWORD winPid;
        GetWindowThreadProcessId(hWnd, &winPid);
        
        if (winPid == data->targetPid) {
            data->found = true;
            // The lparam targetPid might be accompanied by an instruction, 
            // but we handle specific action in the calling function or pass action flags.
        }
    }
    return true;
}

// Struct to pass action instructions to mass commands
struct ActionData {
    bool hideAll;
};

static BOOL CALLBACK MassActionCallback(HWND hWnd, LPARAM lParam) {
    ActionData* data = reinterpret_cast<ActionData*>(lParam);
    
    int length = GetWindowTextLength(hWnd) + 1;
    char* buffer = new char[length];
    GetWindowText(hWnd, buffer, length);
    string title(buffer);
    delete[] buffer;

    if (title == "Roblox") {
        if (data->hideAll) {
            ShowWindow(hWnd, SW_HIDE);
            PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        } else {
            ShowWindow(hWnd, SW_SHOW);
            ShowWindow(hWnd, SW_RESTORE);
        }
    }
    return true;
}

// Struct for collecting all Roblox PIDs and window visibility states
struct ClientInfo {
    DWORD pid;
    HWND hwnd;
    bool isVisible;
};

static BOOL CALLBACK CollectPidsCallback(HWND hWnd, LPARAM lParam) {
    vector<ClientInfo>* clients = reinterpret_cast<vector<ClientInfo>*>(lParam);
    
    int length = GetWindowTextLength(hWnd) + 1;
    char* buffer = new char[length];
    GetWindowText(hWnd, buffer, length);
    string title(buffer);
    delete[] buffer;

    if (title == "Roblox") {
        DWORD winPid;
        GetWindowThreadProcessId(hWnd, &winPid);
        
        // Avoid adding duplicate windows for the same process if multiple exist
        bool exists = false;
        for (const auto& client : *clients) {
            if (client.pid == winPid) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            clients->push_back({winPid, hWnd, (IsWindowVisible(hWnd) != 0)});
        }
    }
    return true;
}

// Helper function to target a specific window action by PID
void ActionByPid(DWORD pid, bool hide) {
    // We reuse CollectPidsCallback to find the exact HWND for the PID
    vector<ClientInfo> clients;
    EnumWindows(CollectPidsCallback, reinterpret_cast<LPARAM>(&clients));
    
    bool found = false;
    for (const auto& client : clients) {
        if (client.pid == pid) {
            found = true;
            if (hide) {
                ShowWindow(client.hwnd, SW_HIDE);
                PostMessage(client.hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                cout << "[+] Hidden Roblox client with PID: " << pid << endl;
            } else {
                ShowWindow(client.hwnd, SW_SHOW);
                ShowWindow(client.hwnd, SW_RESTORE);
                cout << "[+] Showing Roblox client with PID: " << pid << endl;
            }
            break;
        }
    }
    if (!found) {
        cout << "[-] No active Roblox client found with PID: " << pid << endl;
    }
}

int main() {
    cout << "===========================================" << endl;
    cout << "          RBX SILENT COMMAND PANEL         " << endl;
    cout << "===========================================" << endl;
    cout << "Available Commands:" << endl;
    cout << "  pids           - List all open Roblox PIDs and status" << endl;
    cout << "  hide <pid>     - Hide a specific Roblox client" << endl;
    cout << "  show <pid>     - Show a specific Roblox client" << endl;
    cout << "  hideall        - Hide all active Roblox clients" << endl;
    cout << "  showall        - Show all active Roblox clients" << endl;
    cout << "  exit           - Close the panel" << endl;
    cout << "===========================================" << endl;

    string inputLine;
    while (true) {
        cout << "\nrbx_panel> ";
        if (!getline(cin, inputLine)) break;
        if (inputLine.empty()) continue;

        stringstream ss(inputLine);
        string command;
        ss >> command;

        if (command == "exit") {
            break;
        } 
        else if (command == "pids") {
            vector<ClientInfo> clients;
            EnumWindows(CollectPidsCallback, reinterpret_cast<LPARAM>(&clients));
            
            if (clients.empty()) {
                cout << "No active Roblox clients detected." << endl;
            } else {
                cout << left << setw(10) << "PID" << "STATUS" << endl;
                cout << "----------------------" << endl;
                for (const auto& client : clients) {
                    cout << left << setw(10) << client.pid 
                         << (client.isVisible ? "Visible" : "Hidden (Silent)") << endl;
                }
            }
        } 
        else if (command == "hideall") {
            ActionData data = { true };
            EnumWindows(MassActionCallback, reinterpret_cast<LPARAM>(&data));
            cout << "[+] All Roblox windows have been requested to hide." << endl;
        } 
        else if (command == "showall") {
            ActionData data = { false };
            EnumWindows(MassActionCallback, reinterpret_cast<LPARAM>(&data));
            cout << "[+] All Roblox windows have been restored." << endl;
        } 
        else if (command == "hide") {
            DWORD pid;
            if (ss >> pid) {
                ActionByPid(pid, true);
            } else {
                cout << "[-] Usage: hide <pid>" << endl;
            }
        } 
        else if (command == "show") {
            DWORD pid;
            if (ss >> pid) {
                ActionByPid(pid, false);
            } else {
                cout << "[-] Usage: show <pid>" << endl;
            }
        } 
        else {
            cout << "[-] Unknown command. Type 'pids', 'hide <pid>', 'show <pid>', 'hideall', or 'showall'." << endl;
        }
    }

    return 0;
}
