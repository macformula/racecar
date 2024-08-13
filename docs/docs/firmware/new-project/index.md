# Create a New Project

A __project__ is a compilable program that will run on a single device. It should have one `main()` function. Each ECU in our vehicle has its own project.

The simplest project is Blink (see `firmware/projects/Demo/Blink`). This project turns an indicator on and off indefinitely.

In this tutorial, we will __recreate the Blink project from scratch__ for multiple platforms. You will learn how projects are structured, how to configure an CubeMX for the stm32f767 platform, and how to build and run a project.

!!! note

    Unless otherwise mentioned, all file paths are relative to `racecar/firmware`.

## Prepare the project folder

All projects are stored under the `projects/` directory. They can be grouped into subfolders. For example, all demo projects are stored under `projects/Demo` and all EV5 firmware projects are in `projects/EV5`.

1. Create a new folder to hold the project at `projects/MyBlink`.

2. Add a `README.md` file explaining the purpose of this project.

    ```markdown title="projects/MyBlink/README.md"
    # MyBlink

    Recreate the `Demo/Blink` project by following the tutorial at
    <https://macformula.github.io/racecar/firmware/new-project>.
    ```

3. Add a `platforms/` folder to the project. We will populate the platform implementations later.

You should now have the following directory structure.

    projects/
    └── MyBlink/
        ├── platforms/
        └── README.md

## Write the app-level code

A key feature of our firmware architecture is the complete abstraction of the app-level code from any specfic platform implementation.

> See the [Architecture](../architecture.md) article for a refresher on our firmware code structure.
