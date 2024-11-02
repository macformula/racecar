# :material-key: SSH and Commit Signing

This tutorial will guide you through setting up SSH keys and commit signing for GitHub. SSH keys are a more secure way to connect to GitHub, rather than other protocols like HTTPS. Commit signing is used to verify that commits are coming from you and have not been tampered with.

!!! warning
    You must have a verified email address on GitHub to add SSH keys and sign commits.

## Create an SSH Key

=== "Windows"

    In an admin PowerShell terminal, enter the following command:

    ```text
    ssh-keygen -t ed25519 -C "your_github_email@example.com"
    ```

    Save to the default location `c:/Users/YOU/.ssh/id_ed25519` and do not enter a passphrase. This will allow you to use the key without entering a password every time you push to GitHub.

    Continue with the following commands:

    ```text
    type c:/Users/YOU/.ssh/id_ed25519 | clip
    Get-Service -Name ssh-agent | Set-Service -StartupType Manual
    Start-Service ssh-agent
    ssh-add c:/Users/YOU/.ssh/id_ed25519
    ```

=== "Linux/Mac"

    In a terminal window, enter the following command:

    ```text
    ssh-keygen -t ed25519 -C "your_github_email@example.com"
    ```

    Save to the default location `~/.ssh/id_ed25519` and do not enter a passphrase. This will allow you to use the key without entering a password every time you push to GitHub.

    Continue with the following commands:

    ```text
    pbcopy < ~/.ssh/id_ed25519.pub
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_ed25519
    ```

To add the key to your GitHub account, go to <https://github.com/settings/ssh/new>.

- Name your key something descriptive.
- Select "Authentication Key" as the key type.
- Paste the contents of `id_ed25519.pub` into the "Key" box.
- Click "Add SSH Key".

Open another terminal to test your connection to GitHub by entering the following command:

```text
ssh -T git@github.com
```

??? Failure "Windows: `ssh` not recognized"

    Older versions of Windows do not include SSH so it must be installed separately.

    1. Open the __Settings__ app. Go to __System__ then __Optional Features__.
    2. Click __Add a feature__ and search for "OpenSSH."
    3. Install the feature. Restart your computer.
    4. Add the OpenSSH directory to your PATH.

        The default location is `C:\Windows\System32\OpenSSH`

    Open a new terminal. You should be able to execute `ssh`.

Type "yes" when it asks if you want to continue connecting.
You should see a message like "Hi username! You've successfully authenticated, but GitHub does not provide shell access."

Additional SSH GitHub documentation can be found [here](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent) and [here](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/testing-your-ssh-connection).

## Enable Commit Signing

To add a signing key to your GitHub account, go to <https://github.com/settings/ssh/new>.

- Name your key something descriptive.
- Select "Signing Key" as the key type.
- Paste the contents of `id_ed25519.pub` into the "Key" box.
- Click "Add SSH Key".

Update your git configuration in your terminal to automatically sign commits with your key:

=== "Windows"
    ```text
    git config --global user.signingkey c:/Users/YOU/.ssh/id_ed25519
    git config --global gpg.format=ssh
    git config --global commit.gpgsign true
    ```

=== "Linux/Mac"
    ```text
    git config --global user.signingkey ~/.ssh/id_ed25519
    git config --global gpg.format=ssh
    git config --global commit.gpgsign true
    ```

Additional commit signing GitHub documentation can be found [here](https://docs.github.com/en/authentication/managing-commit-signature-verification/telling-git-about-your-signing-key).
