# AppUserModelID Stopgap Tools

The AppUserModelID Stopgap Tools allow you to pin the same application with different parameters and control their grouping on the taskbar.

Windows is grouping windows on the taskbar based on [Application User Model IDs (AppUserModelIDs)](https://learn.microsoft.com/en-us/windows/win32/shell/appids). The AppUserModelID Stopgap Tools provides utility commands for tweak that grouping behavior.

The following demo shows pinned two [Obsidian](https://obsidian.md) icons on the taskbar with different vaults:

![AppUserModelID Stopgap Tools demo animation](https://raw.githubusercontent.com/tksh164/whatnot/main/aumid-stopgap-tools/media/apumid-stopgap-tools-demo.gif)

> [!NOTE]
> Essentially, the window grouping is managed by the app itself (AppUserModelIDs managed by the app itself). The AppUserModelID Stopgap Tools change it from externally, that is unorthodox way.

## Prerequisites

- The tools were tested on the latest version of Windows 11 with the latest updates.
    - Also, it may work on the other Windows versions.
- The tools were tested with [Obsidian](https://obsidian.md) and [Visual Studio Code](https://code.visualstudio.com/).
    - As a mechanism, it can work with other apps too.

## Installation

1. Download the [latest release zip file](https://github.com/tksh164/aumid-stopgap-tools/releases/latest).

2. Unblock the downloaded zip file by check **Unblock** from the zip file's property or using the [Unblock-File](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.utility/unblock-file) cmdlet.
    
    ```powershell
    Unblock-File aumid-stopgap-tools-x.y.z.zip
    ```
    
3. Extract files from the downloaded zip file. You can extract files by the **Extract All...** context menu in the File Explorer or using the [Expand-Archive](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.archive/expand-archive) cmdlet.

    ```powershell
    Expand-Archive aumid-stopgap-tools-x.y.z.zip
    ```

4. Put the extracted files anywhere you like.

    If you don't need this tools anymore, you can uninstall it by deleting the tools' folder.

## Usage

- [Usage for v0.1.0](./docs/usage-v0.1.0.md)

## License

Copyright (c) 2024 Takeshi Katano. All rights reserved. This software is released under the [MIT License](https://github.com/tksh164/aumid-stopgap-tools/blob/main/LICENSE).

Disclaimer: The codes stored herein are my own personal codes and do not related my employer's any way.
