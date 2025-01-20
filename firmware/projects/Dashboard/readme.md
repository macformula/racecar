# How to Run Dashboard LVGL Simulator on WSL

This guide outlines the steps to run the Dashboard LVGL simulator on Windows Subsystem for Linux (WSL).

## Setup Steps

1. **Clone ver 8.0.0 of LVGL VSCode Port Repository:**
   Follow the instructions in this repo to install dependencies
   ```bash
   https://github.com/lvgl/lv_port_pc_vscode


2. **Replace Makefile:**
   - Navigate to the `lv_port_pc_vscode` directory:
     ```bash
     cd lv_port_pc_vscode
     ```
   - Replace the existing `Makefile` with the one provided here

3. **Replace Files in Source Directory:**
   - Navigate to `lv_port_pc_vscode/main/src`:
     ```bash
     cd main/src
     ```
   - Replace the contents of this directory with the files found in `dashboard_files` (ensure you have these files available).

4. **Run the Debug Test:**
   - Open the project folder in Visual Studio Code via the WSL environment.
   - Click F5 to run