# Development Environment Setup

Follow these steps to begin developing in `racecar/firmware`.

!!! tip
    You can copy a command by clicking the :material-content-copy: icon

        echo "Copy Me --->"

## Dependencies

=== "Windows"

    1. We will use Chocolatey to install most dependencies. Go to <https://chocolatey.org/install#individual> and follow the instructions under "Install Chocolatey for Individual Use".

    2. Open a Command Prompt __as administrator__ and run

        ```text
        choco upgrade git msys2 make cmake -y
        ```

        !!! warning "Git Bash"

            The Git Bash terminal shell is included when you install `git`. This shell emulates the Linux bash shell, allowing you to use commands like `grep`, `ls`, `rm` and many more on Windows.

            If you are on Windows, you need to use Git Bash as your shell when developing in `racecar/`. (i.e. not Command Prompt or Powershell).

            In VSCode, you can change your default shell by pressing ++f1++ and entering `>Terminal: Select Default Profile` and selecting your desired shell program.
        
    3. Install the newest version of Python from <https://www.python.org/downloads/>. When installing, ensure `Add python.exe to PATH` is checked.

    4. Open the MSYS2 directory by searching `msys2` in the Start Menu :material-microsoft-windows: and choosing "Open file location." Run `msys2.exe` to open a terminal and install the GNU toolchain with

        ```bash
        pacman -Syuu
        pacman -S mingw-w64-x86_64-toolchain
        ```

        Press ++enter++ when prompted to "Enter a selection."

        Add the MSYS2 mingw64 binary and library directories to your PATH. These paths will be of the form
        
        ```text
        C:\path-to-msys2\mingw64\bin
        C:\path-to-msys2\mingw64\lib
        ```

=== "Linux"

    1. Set up the Kitware APT repository <https://apt.kitware.com/>.

        > This allows `apt` to find new versions of CMake.

    2. In your terminal, run

        ```bash
        sudo apt-get update
        sudo apt-get install software-properties-common python3-launchpadlib
        sudo add-apt-repository ppa:deadsnakes/ppa
        sudo apt-get update
        sudo apt-get install git-all build-essential cmake python3.12 wget gcc-13
        ```

### Arm Toolchain

Install the Arm GNU Toolchain (v13 or newer) from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.

1. Find the __<ARCHITECTURE\> hosted cross toolchains__ section for your computer's architecture then download and install the __AArch32 bare-metal target (arm-none-eabi)__.

    > There are multiple downloads available in this section. Choose the installer: `.exe` on Windows, `.tar.xz` on Linux, `.pkg` on MacOS.

2. Add the folder containing the binaries to your path.

### Verify Installation

Check that all programs were installed and have an acceptable version.

_Do not copy the `# version comments`._

```{.bash .no-copy}
git --version     # >= 2.40
python --version  # >= 3.10, use python3 on Linux / Mac
make --version    # >= 4.0
cmake --version   # >= 3.27
g++ --version     # >= 13
arm-none-eabi-g++ --version     # >= 13.0
```

## Install STM32 Tools

### Create an ST Account

Go to [www.st.com](https://www.st.com), click on the account :material-account: icon in the top right and create an account. You will need this username and password later on.

### STM32CubeMX

CubeMX is a program which generates configuration code for our microcontrollers.

1. Download version 6.12.0 from <https://www.st.com/en/development-tools/stm32cubemx.html> and install it. You may need to sign in with your ST account.

    !!! warning

        You must install this version __exactly__ (not even 6.12.1). Using a different version will cause issues when opening files.

2. Open the install path which contains the `STM32CubeMX` executable and `jre/` directory. Add this directory to your PATH.

3. Open a terminal in that directory and run CubeMX in "interactive" mode to login.

    === "Windows (Git Bash)"

            jre/bin/java -jar STM32CubeMX.exe -i

    === "Linux"

            jre/bin/java -jar STM32CubeMX -i
    === "MacOs (Terminal)"
            jre/Contents/Home/bin/java -jar STM32CubeMX -i

4. Wait for the program to stop printing to your terminal. Press ++question+enter++ to display the `MX>` prompt. Login with your `username` and `password`.

    ```{.text .no-copy}
    login username password y
    ```

    Do not omit the `y` at the end!

5. Install the STM32F7 firmware pack.

    ```text
    swmgr install stm32cube_f7_1.17.2 ask
    ```

    You can close CubeMX now by typing `exit`.

## Clone the Repository

Navigate to a directory where you would like to hold the `racecar` repo (I used `C:\Formula\repos`). Run

    git clone --recurse-submodules https://github.com/macformula/racecar.git

## Pre-Commit Setup

We use [pre-commit](https://pre-commit.com/) to run formatting and code checks before the code is pushed.

```bash
pip install pre-commit
pre-commit install
```

## Install CANgen

Create a Python virtual environment for CANgen. Navigate into `racecar/firmware` and run.

=== "Windows (Git Bash)"

    ```text
    python -m venv .env
    source .env/Scripts/activate
    ```

=== "Linux / Mac"

    ```bash
    python3 -m venv .env
    source .env/bin/activate
    ```

The second command "activates" the virtual environment. You will see `(.env)` beside your terminal prompt when it is activated. __It must be activated before building any project.__

With the environment activated, change into `racecar/` and install CANgen:

```bash
pip install -e scripts/cangen
```

> The `-e` flag is _very_ important. It installs CANgen as an editable package which means you won't have to reinstall when the package is changed.

---

You can now start developing in `racecar`! However, you should configure your IDE, so check out

- [VS Code Setup](vscode.md)
