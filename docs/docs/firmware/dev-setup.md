# Development Environment Setup

Follow these steps to begin developing in `racecar/firmware`.

!!! tip
    You can copy a command by clicking the :material-content-copy: icon

        echo "Copy Me --->"

## Dependencies

=== "Windows"

    1. We will use Chocolatey to install most dependencies. Go to <https://chocolatey.org/install#individual> and follow the instructions under "Install Chocolatey for Individual Use".

        Open a Command Prompt __as administrator__ and run

        ```text
        choco upgrade git msys2 make cmake -y
        ```
        
    2. Install the newest version of Python from <https://www.python.org/downloads/>. When installing, ensure `Add python.exe to PATH` is checked.

    3. Open an MSYS2 terminal by searching `msys2` in the Start Menu :material-microsoft-windows:. Install the GNU toolchain with

        ```bash
        pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
        ```

        Press ++enter++ when prompted to "Enter a selection."

        Add the MSYS2 ucrt64 binary directory to your PATH. You can find this path by searching `msys2` in the Start menu, clicking "Open File Location" then following `ucrt64/bin`.

    4. Install the Arm GNU Toolchain from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.
        
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
        sudo apt-get install git-all build-essential cmake python3.12 wget
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

_Do not copy the `# version comments`_

```{.bash .no-copy}
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

    === "Windows"

            jre\bin\java -jar STM32CubeMX.exe -i

    === "Linux"

            jre/bin/java -jar STM32CubeMX -i

4. Wait for the program to stop printing to your terminal. Press ++question+enter++ to display the `MX>` prompt. Login with your `username` and `password`.

        MX> login username password y

    Do not omit the `y` at the end! You can close CubeMX now.

### STM32CubeProgrammer

Download and install version 2.16 or newer from <https://www.st.com/en/development-tools/stm32cubeprog.html>.

It does not need to be on your PATH.

## Clone the Repository

Navigate to a directory where you would like to hold the `racecar` repo (I used `C:\Formula\repos`). Run

    git clone --recurse-submodules https://github.com/macformula/racecar.git

---

### clangd

??? info "What is `clangd`?"

    [`clangd`](https://clangd.llvm.org/) is our official C/C++ language server. It is __not required__ but will make your development experience much nicer. We have configured CMake to export your most recent compilation's build commands meaning you don't need to manually configure include paths or compiler flags for each project.

    Whenever you build a project with the Makefile, `clangd` will see the new `build/compile_commands.json` and immediately update your IDE's include paths. When you switch which project or platform you are developing for, simply rebuild the project and your development environment will be automatically prepared.

__Version__ 16.0.2

> We hope to update this version soon. See [Issue 93](https://github.com/macformula/racecar/issues/93).

Download `clangd-{OS}-16.0.2.zip` from <https://github.com/clangd/clangd/releases?q=16.0.2> and unzip.

The executable does __not__ need to be on your PATH. Place the binary somewhere convenient and reference it in your IDE (see [IDE Setup](#ide-setup)).

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

### gRPC

!!! warning "Optional Dependency"

    Within the racecar repo, gRPC is only used by the SIL Client under `firmware/validation/sil/`. The client is used by the Raspberry Pi MCAL to interact with the [HIL / SIL](https://github.com/macformula/hil).

    You only need gRPC if you will be working on the HIL. Otherwise, do not worry about installing it.

You will need a Unix development environment (Unix machine, WSL, or remote into the Raspberry Pi).

Go through the [gRPC C++ Quickstart Guide](https://grpc.io/docs/languages/cpp/quickstart/). Build the example project.

## IDE Setup

_If you use a different IDE, consider [adding instructions](../tutorials/site-dev.md) for setting it up!_

=== ":material-microsoft-visual-studio-code: VS Code"

    __`clangd` Setup__

    1. If you have the [Microsoft C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), disable it for the `racecar` workspace.

        > Open the Extensions pane with ++ctrl+shift+x++, right click on the C/C++ extension and select `Disable (Workspace)`.
   
    1. Install the [`clangd` extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) from LLVM.   

    1. Provide the `clangd` extension settings with the path to the `clangd` executable installed [earlier](#clangd).

        1. Select the `clangd` extension and open :octicons-gear-24: Extension Settings.
        
        1. Paste the full `clangd` executable path in the "Clangd: Path" setting.
