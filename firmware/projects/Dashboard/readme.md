# How to Run Windows Simulation

1. **Clone the LVGL Visual Studio Windows Port from GitHub**
   - Run the following command:
     ```bash
     git clone --recurse-submodules https://github.com/lvgl/lv_port_pc_visual_studio.git
     ```

2. **Move the Dashboard Files**
   - Move the `dashboard-files` folder into the cloned LVGL port, specifically into the `LvglWindowsSimulator` folder.

3. **Open the Project in Visual Studio**
   - Open `LvglWindowsSimulator.vcxproj`.

4. **Add Dashboard Files to the Solution**
   - Right-click `LvglWindowsSimulator` in the Solution Explorer.
   - Click **Add** and then **Add Existing Item**.
   - Select and add all contents of the `dashboard-files` folder to the solution.

5. **Replace the Main Code**
   - Replace the code in `LvglWindowsSimulator.cpp` with the code from `dashboard-main.cpp`.

6. **Compile and Run**
   - Click the **Local Windows Debugger** icon to compile and run the simulation.
