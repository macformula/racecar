# VS Code Setup

## Language Integration with clangd

[`clangd`](https://clangd.llvm.org/) is a C++ language server that provides intelligent code completion and errors messages.

??? info "Why clangd over other language servers?"
    clangd is smart. It analyzes our build system to known exactly how all of the files fit together so you never need to manually configure include paths or compiler flags.

    Whenever you build a project with the Makefile, `clangd` will see the new `build/compile_commands.json` and immediately update your IDE's include paths. When you start developing for a different project or platform, simply rebuild the project and your language server will automatically update.

### Installation

1. Install the official clangd VS Code extension from LLVM <https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd>.
2. Open the VS Code command palette with ++ctrl+shift+p++. Search and run `clangd: Download Language Server`.
3. Disable any other C++ language extensions that may conflict with clangd, such at the [Microsoft C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).
4. Create an empty file named `.clangd` in the `firmware/` directory. This ensures clangd knows where to find the `build/compile_commands.json` file.

### clangd Configuration

Configure the `firmware/.clangd` file for your architecture.

=== ":material-microsoft-windows: Windows"

    ```yaml
    CompileFlags:
        Add:
            - --target=x86_64-w64-mingw64
    ```

=== ":material-apple: Mac M-Series"

    ```yaml
    CompileFlags:
        Add:
            - --target=arm64-apple-darwin22.6.0
    ```
    
> The `--target` flag should be `#commented out` unless you are compiling for the `cli` platform.

For more configuration options, see the official clangd docs <https://clangd.llvm.org/config>.

### Use clangd as a formatter

You can use clangd to automatically format your code whenever you save your file. This will prevent your commits being rejected by the GitHub action.

1. Open any `.cc` file in the repo, such as `projects/Demo/Blink/main.cc`.
2. Configure clangd as your default C++ formatter:
    1. Open the VS Code command palette. Run `Format Document With...`
    2. Select `Configure Default Formatter -> clangd`
3. Edit your settings to enable format-on-save:
    1. Open the VS Code command palette. Run `Preferences: Open Workspace Settings (JSON)`
    2. Edit the `[cpp]` section (create it if necessary) to add

    ```json
    "[cpp]": {
        "editor.formatOnSave": true,
    },
    ```

To verify the formatter is working, open any `.cc` file and type some poorly-formatted code. When you ++ctrl+s++ save, the code should be immediately formatted.

<div class="grid" markdown>
```c++ title="Before"
int x      =     100;
for(int i = 0;
    i<10; i++)
{
        x += i;
        }
```

```c++ title="After"
int x = 100;
for (int i = 0; i < 10; i++) {
    x += i;
}
```

</div>
