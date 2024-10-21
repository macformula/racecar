# Cangen
Cangen is a cli package used to generate C++/C files from a corresponding dbc file. This tool automates the can generation process and ensures can generation is consistent between all projects. 

### Installation
First install the cangen package through the terminal:

```
pip install racecar/scripts/cangen
```
The directory where you run this command does not matter, as long as the path specified leads to the directory containing the pyproject.toml for cangen.
For example, if you are in the racecar directory already, then you could run 
```
pip install scripts/cangen
```

The directory where you run this command does not matter, as long as the path specified leads to the directory containing the pyproject.toml for cangen.
For example, if you are in the racecar directory already, then you could run 
```
pip install scripts/cangen
```


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