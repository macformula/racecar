# Development Environment Setup

Follow these steps to begin developing in `racecar/firmware`.

## Dependencies

Unless otherwise mentioned, all programs must be accessible on your [PATH](https://en.wikipedia.org/wiki/PATH_(variable)).

The Windows instructions assume that you have installed [Chocolatey](https://chocolatey.org/install).

### GNU Make

__Version__ 4.0 or newer

=== "Windows"

        choco install make

=== "Linux"

        sudo apt-get install build-essential

!!! tip "Verification"

        make --version

---

### CMake

__Version__ 3.27 or newer

=== "Windows"

        choco install cmake

=== "Linux"

        sudo apt-get install cmake

    > If this installs an old version (as it did for my Ubuntu 20.04 LTS), you may need to follow the steps at <https://apt.kitware.com/> to use the APT repository hosted by the developers of CMake.

!!! tip "Verification"

        cmake --version

---

### Arm GNU Toolchain

__Version__ 13.2.rel1 or newer

Download from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.

Choose an installer from the "AArch32 bare-metal target (arm-none-eabi)" section. Ensure the executables are available on your PATH.  

!!! tip "Verification"

        arm-none-eabi-gcc --version

---

### STM32CubeMX

__Version__ 6.12.0

!!! warning

    You must install this version __exactly__ (not even 6.12.1). Using a different version will cause issues when opening files.

Download from <https://www.st.com/en/development-tools/stm32cubemx.html#st-get-software>.

!!! tip "Verification"

    Running `stm32cubemx` should open the program.

---

### Java Runtime Environment

__Version__ 17 or newer

=== "Windows"

    No installation needed as it comes with CubeMX.

=== "Linux"

        sudo apt install openjdk-17-jre-headless

> [Issue 142](https://github.com/macformula/racecar/issues/142) may remove this dependency.

---

### STM32CubeProgrammer

__Version__ 2.16.0 or newer

Download from <https://www.st.com/en/development-tools/stm32cubeprog.html>.

It does not need to be on your PATH.

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

## TaskFile Setup

__version__ 3.37.2 

=== "Windows"

        choco install go-task  

    

