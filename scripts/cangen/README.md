# Cangen

Generates C++ files from a corresponding dbc file. Ensures repeatable generation between all projects.

## Installation

From the `racecar/` directory, run

```bash
pip install scripts/cangen
```

## Usage

To generate CAN code for a project, execute `cangen` and pass the project folder. The project folder is the one which contains `config.yaml`.

## Example

If you are in the `racecar/firmware/` directory, you could generate `FrontController` code with

```bash
cangen projects/FrontController
```

This command will generate code in a `generated/can/` subfolder of the project.

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

## Development

We use Poetry to manage this package. Install the development environment with

```bash
poetry install
```

To run the tests:

```bash
poetry run pytest
```
