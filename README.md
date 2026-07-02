# RBXSilent2 (Interactive Command Panel) 🚀

A lightweight C++ command-line utility designed to hidden-minimize Roblox game clients, dropping their GPU utilization to near-zero. 

This project is an interactive extension built upon the original core concept and codebase created by **toastering** in the repository **[toastering/rbxsilent](https://github.com/toastering/rbxsilent)**.

---

## 📥 Direct Download

Get the latest pre-compiled executable directly from the Releases page:

[<img src="https://img.shields.io/badge/Download-rbxsilent.exe-brightgreen?style=for-the-badge&logo=windows" alt="Download Executable">](https://github.com/lilmond/rbxsilent2/releases/download/v1.0.0/rbxsilent2_v1.0.0.exe)

---

## 🧠 How It Works (GPU Optimization)
When running multiple game accounts, your graphics card heavily strains from rendering background windows. This tool solves that by utilizing a clever two-step Windows API sequence:
1. `ShowWindow(hWnd, SW_HIDE)` completely pulls the window from the Windows Desktop Window Manager (DWM) compositing loop.
2. `PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0)` tricks the engine into an absolute idle background state.

Because the game engine halts frame buffer updates (`Present()`), your **GPU utilization, temperatures, and power draw plunge to near-zero**, freeing up your hardware for other tasks.

---

## 💻 Available Commands

Launch the executable to open the interactive user console (`rbx_panel>`). You can manage your clients dynamically using these commands:

| Command | Syntax | Description |
| :--- | :--- | :--- |
| **List PIDs** | `pids` | Scans the system and lists all open Roblox PIDs alongside their current status (Visible vs. Hidden). |
| **Hide Client** | `hide <pid>` | Hides a specific client and forces it into a zero-rendering background state. |
| **Show Client** | `show <pid>` | Restores a specific hidden client back to your desktop. |
| **Hide All** | `hideall` | Instantly minimizes and hides every active client instance to free up GPU resources. |
| **Show All** | `showall` | Restores all hidden clients back to visibility. |
| **Exit** | `exit` | Safely closes the management panel. |

### Quick Example Usage

```text
rbx_panel> pids
PID       STATUS
----------------------
14304     Visible
8712      Visible

rbx_panel> hide 14304
[+] Hidden Roblox client with PID: 14304

rbx_panel> pids
PID       STATUS
----------------------
14304     Hidden (Silent)
8712      Visible
```

### 🔨 Compilation Instructions
If you prefer to compile the binary yourself from the raw source code using MinGW/w64devkit, run the following command in your terminal:

```
g++ rbxsilent2.cpp -o rbxsilent2.exe
```

### ⚠️ Important Disclaimer
This utility manipulates window visibility states using standard Windows API protocols. While the source code is entirely transparent and safe from traditional malware, using automated third-party window manipulation tools alongside modern game anti-cheat systems (like Byfron/Hyperion) always carries an inherent risk of automated flags or account restrictions. Use responsibly.
