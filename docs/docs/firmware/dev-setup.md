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

    5. Install the Arm GNU Toolchain from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.

        1. Download the `.exe` installer under "Windows hosted cross toolchains -> AArch32 bare-metal target (arm-none-eabi)".
        2. Run the installer.
        3. After the installer finishes, check `Add path to environment variable.`

=== "Linux"

    1. Set up the Kitware APT repository <https://apt.kitware.com/>.

        > This allows `apt` to find new versions of CMake.

    2. In your terminal, run

        ```bash
        sudo apt-get update
        sudo apt-get install software-properties-common
        sudo add-apt-repository ppa:deadsnakes/ppa
        sudo apt-get update
        sudo apt-get install git-all build-essential cmake python3.12 wget gcc-13
        ```

    3. Install the Arm GNU Toolchain.

        1. Download and unzip the x86_64 Linux arm-none-eabi toolchain binaries.

                wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
                sudo tar xf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz -C /usr/share
                rm arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz

        2. Add the binaries to your profile PATH.

            Open `~/.profile` in a text editor and add this to the end of the file.

                PATH="/usr/share/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin:$PATH"

### Verify Installation

Check that all programs were installed and have an acceptable version.

_Do not copy the `# version comments`._

```{.bash .no-copy}
git --version     # >= 2.40
python --version  # >= 3.10, use python3 on Linux / Mac
make --version    # >= 4.0
cmake --version   # >= 3.27
g++ --version     # >= 10
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

### STM32CubeProgrammer

Download and install version 2.16 or newer from <https://www.st.com/en/development-tools/stm32cubeprog.html>.

## Clone the Repository

Navigate to a directory where you would like to hold the `racecar` repo (I used `C:\Formula\repos`). Run

    git clone --recurse-submodules https://github.com/macformula/racecar.git

## Install CANgen dependencies

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

Finally, with the environment activated, change into `racecar/` and install CANgen:

```bash
pip install scripts/cangen
```

You can now start developing in `racecar`! However, I recommend you configure your IDE with `clangd`, so continue to the next section.

## IDE Integration

_This section is optional but highly recommended._

### clangd

??? info "What is `clangd`?"

    [`clangd`](https://clangd.llvm.org/) is a C++ language server that provides intelligent code completion and errors by analyzing compiler commands C/C++ language server. We have configured CMake to export your most recent compilation's build commands meaning you don't need to manually configure include paths or compiler flags for each project.

    Whenever you build a project with the Makefile, `clangd` will see the new `build/compile_commands.json` and immediately update your IDE's include paths. When you switch which project or platform you are developing for, simply rebuild the project and your development environment will be automatically prepared.

Download and unzip `clangd-{OS}-16.0.2.zip` from <https://github.com/clangd/clangd/releases?q=16.0.2> under "Assets."

Place the executable somewhere on your PATH. Follow [these instructions](https://clangd.llvm.org/installation#editor-plugins) to connect it to your IDE.

__Important:__ Create an empty file named `.clangd` in the `firmware/` directory. This file indicates the "root" directory of the project, so it will look for `compile_commands.json` in `firmware/build`.

!!! note "`clangd` Configuration"

    You may optionally configure `clangd` with the `firmware/.clangd` file. See <https://clangd.llvm.org/config>.
    
    === ":material-microsoft-windows: Windows"
    
        Use this configuration to help `clangd` find system header files.

        ```yaml
        CompileFlags:
            Add: --target=x86_64-w64-mingw64
        ```

    === ":material-apple: Mac M-Series"

        On Mac M series chips, this configuration should be used.

        ```yaml
        CompileFlags:
            Add: --target=arm64-apple-darwin22.6.0
        ```

        
## Additional Dependencies

### gRPC

!!! warning "Optional Dependency"

    _You probably don't need this._

    Within the racecar repo, gRPC is only used by the SIL Client under `firmware/validation/sil/`. The client is used by the Raspberry Pi MCAL to interact with the [HIL / SIL](https://github.com/macformula/hil).

    You only need gRPC if you will be working on the HIL. Otherwise, do not worry about installing it.

You will need a Unix development environment (Unix machine, WSL, or remote into the Raspberry Pi).

Go through the [gRPC C++ Quickstart Guide](https://grpc.io/docs/languages/cpp/quickstart/). Build the example project.

## Pre-Commit Setup

We use `pre-commit` hooks to run formatting and code checks before the code is pushed.

### Installing Pre-Commit

1. Install `pre-commit` via pip:

    ```bash
    pip install pre-commit
    ```

2. Install the git hooks by running this command in the `racecar` directory:

    ```bash
    pre-commit install
    ```

    This will install the hooks so they run automatically when you use `git commit`.
