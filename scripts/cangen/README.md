# Cangen
Cangen is a cli package used to generate C++/C files from a corresponding dbc file. This tool automates the can generation process and ensures can generation is consistent between all projects. 

### Installation
First install the cangen package through the terminal:

```
pip install racecar/scripts/cangen
```
Make sure to specify the relative path to the cangen directory. 

### Example usage
To generate code for a project, like FrontController:
```
cangen racecar/firmware/projects/EV5/FrontController
```
The file path should be the relative path to the project you are looking to generate code for.
This command will generate code in a generated/can folder inside the selected projects directory. 

```
FrontController
├─ fc_docs
├─ generated/can
│  ├─ .gitignore
│  ├─ pt_can_messages.h
│  ├─ pt_msg_registry.h
│  ├─ veh_can_messages.h
│  └─ veh_msg_registry.h
├─ inc
├─ platforms
├─ vehicle_control_system
├─ CMakeLists.txt
├─ config.yaml
├─ main.cc
└─ README.md
```