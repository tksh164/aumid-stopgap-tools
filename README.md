# AppUserModelID tweak tools

The AppUserModelID tweak tools allow you to pin the same application with different parameters on the taskbar.

Windows is grouping windows on the taskbar based on [Application User Model IDs](https://learn.microsoft.com/en-us/windows/win32/shell/appids). The AppUserModelID tweak tools provides utility commands for tweak that grouping behavior.

## Installation

1. Download [the app's latest release zip file](https://github.com/tksh164/aumid-tweakers/releases/latest).

2. Unblock the downloaded zip file by check **Unblock** from the file's property or using [Unblock-File](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.utility/unblock-file) cmdlet.
    
    ```powershell
    Unblock-File aumid-tweakers-x.y.z.zip
    ```
    
3. Extract files from the downloaded zip file. You can extract files by the **Extract All...** context menu in the File Explorer or using [Expand-Archive](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.archive/expand-archive) cmdlet.

    ```powershell
    Expand-Archive aumid-tweakers-x.y.z.zip
    ```

4. Put the extracted files anywhere you like.

    If you don't need this tools anymore, you can uninstall it by deleting the tools' folder.

## Using tools

### mklnkwaumid

The **mklnkwaumid** creates a shortcut file (`.lnk`) that has a specified Application User Model ID.

```
mklnkwaumid <ShortcutFilePath> <AppUserModelID> <Target> [<ParametersForTarget>]
```

- ShortcutFilePath
- AppUserModelID
- Target
- ParametersForTarget (optional)

### runwaumid

The **runwaumid** executes an application then set a specified Application User Model ID to the main windows of the launched application. The **runwaumid** provides three different window finding modes.

#### Find the target window by the partial window title text

<!-- TODO: Write use case -->

```powershell
runwaumid -tp <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-tp`: The window title text partial matching mode. Use the text specified as `WindowTitle` to identify the target window.
- AppUserModelID: The [Application User Model ID](https://learn.microsoft.com/en-us/windows/win32/shell/appids) to set to the target window. The windows that have the same Application User Model ID are grouped in the taskbar.

    The Application User Model ID should has the following [form](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid). It can have no more than 128 characters and cannot contain spaces. Each section should be pascal-cased. `CompanyName` and `ProductName` should always be used, while the `SubProduct` and `VersionInformation` portions are optional. 

    ```
    CompanyName.ProductName[.SubProduct[.VersionInformation]]
    ```

- `WindowTitle`: The partial window title text to use to find the target window.
- `TargetToOpen`: The target to open by shell such as executable files (`.exe`), document files, addresses, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

#### Find the target window by the exact window title text

<!-- TODO: Write use case -->

```powershell
runwaumid -te <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-te`: The window title text exact matching mode. Use the text specified as `WindowTitle` to identify the target window.
- `AppUserModelID`: The [Application User Model ID](https://learn.microsoft.com/en-us/windows/win32/shell/appids) to set to the target window. The windows that have the same Application User Model ID are grouped in the taskbar.

    The Application User Model ID should has the following [form](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid). It can have no more than 128 characters and cannot contain spaces. Each section should be pascal-cased. `CompanyName` and `ProductName` should always be used, while the `SubProduct` and `VersionInformation` portions are optional. 

    ```
    CompanyName.ProductName[.SubProduct[.VersionInformation]]
    ```

- `WindowTitle`: The exact window title text to use to find the target window.
- `TargetToOpen`: The target to open by shell such as executable files (`.exe`), document files, addresses, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

#### Find the target window by the process ID

<!-- TODO: Write use case -->

```powershell
runwaumid -p <AppUserModelID> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-p`: The process ID mode. Use the process ID of the launched process to identify the target window.
- `AppUserModelID`: The [Application User Model ID](https://learn.microsoft.com/en-us/windows/win32/shell/appids) to set to the target window. The windows that have the same Application User Model ID are grouped in the taskbar.

    The Application User Model ID should has the following [form](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid). It can have no more than 128 characters and cannot contain spaces. Each section should be pascal-cased. `CompanyName` and `ProductName` should always be used, while the `SubProduct` and `VersionInformation` portions are optional. 

    ```
    CompanyName.ProductName[.SubProduct[.VersionInformation]]
    ```

- `TargetToOpen`: The target to open by shell such as executable files (`.exe`), document files, addresses, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

<!-- TODO: NOTICE -->

## Typical use case

Typically, you need just two steps.

1. Create a shortcut file (.lnk) using the **mklnkwaumid** to execute the target application with parameters via the **runwaumid**.
2. Drag and drop the shortcut file that was created on the first step to the taskbar.

### Obsidian with different vault

1. Create a shortcut file (.lnk) for each vault using the **mklnkwaumid**. For example, execute the following command in PowerShell.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault1.lnk' Tksh164.Obsidian.Vault1 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault1 \"Vault1 - Obsidian\" obsidian://open?vault=Vault1'
    ```

    - The **mklnkwaumid** creates `D:\temp\Obsidian-Vault1.lnk` file and sets `Tksh164.Obsidian.Vault1` to the shortcut file as an AppUserModelID. You need to specify AppUserModelID that follow the [form](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid) and unique for each group on the taskbar.
    - The `D:\temp\Obsidian-Vault1.lnk` will launches the **runwaumid** with parameters.
    - The **runwaumid** executes `obsidian://open?vault=Vault1` then find the window that has `Vault1 - Obsidian` partially in the windows title text. After found the window, the **runwaumid** sets `Tksh164.Obsidian.Vault1` to the windows as an AppUserModelID.
    - It is important that specify the same AppUserModelID both the shortcut (created by **mklnkwaumid**) and the window (executed via **runwaumid**).

    You can create multiple shortcut files in the same way. For example:

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault2.lnk' Tksh164.Obsidian.Vault2 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault2 \"Vault2 - Obsidian\" obsidian://open?vault=Vault2'
    ```

2. Customize the created shortcut file (e.g. `D:\temp\Obsidian-Vault1.lnk`) as you like. For instance, Icon, Comment, etc.

3. Drag and drop the shortcut file (e.g. `D:\temp\Obsidian-Vault1.lnk`) on the taskbar.

4. You can delete `D:\temp\Obsidian-Vault1.lnk` file after the drag and drop it on the taskbar.

### Visual Studio Code with different workspace

1. Create a shortcut file (.lnk) for each vault using the **mklnkwaumid**. For example, execute the following command in PowerShell.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\VSCode-Workspace1.lnk' Tksh164.VSCode.Workspace1 'C:\bin\runwaumid.exe' '-tp Tksh164.VSCode.Workspace1 \"Workspace1 (Workspace) - Visual Studio Code\" \"C:\Program Files\Microsoft VS Code\Code.exe\" \"D:\VSCode\Workspace1.code-workspace\"'
    ```

    - The **mklnkwaumid** creates `D:\temp\VSCode-Workspace1.lnk` file and sets `Tksh164.VSCode.Workspace1` to the shortcut file as an AppUserModelID. You need to specify AppUserModelID that follow the [form](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid) and unique for each group on the taskbar.
    - The `D:\temp\VSCode-Workspace1.lnk` will launches the **runwaumid** with parameters.
    - The **runwaumid** executes `"C:\Program Files\Microsoft VS Code\Code.exe" "D:\VSCode\Workspace1.code-workspace"` then find the window that has `Workspace1 (Workspace) - Visual Studio Code` partially in the windows title text. After found the window, the **runwaumid** sets `Tksh164.VSCode.Workspace1` to the windows as an AppUserModelID.
    - It is important that specify the same AppUserModelID both the shortcut (created by **mklnkwaumid**) and the window (executed via **runwaumid**).

    You can create multiple shortcut files in the same way. For example:

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\VSCode-Workspace2.lnk' Tksh164.VSCode.Workspace2 'C:\bin\runwaumid.exe' '-tp Tksh164.VSCode.Workspace2 \"Workspace2 (Workspace) - Visual Studio Code\" \"C:\Program Files\Microsoft VS Code\Code.exe\" \"D:\VSCode\Workspace2.code-workspace\"'
    ```

2. Customize the created shortcut file (e.g. `D:\temp\VSCode-Workspace1.lnk`) as you like. For instance, Icon, Comment, etc.

3. Drag and drop the shortcut file (e.g. `D:\temp\VSCode-Workspace1.lnk`) on the taskbar.

4. You can delete `D:\temp\VSCode-Workspace1.lnk` file after the drag and drop it on the taskbar.

## License

Copyright (c) 2024-present Takeshi Katano. All rights reserved. This software is released under the [MIT License](https://github.com/tksh164/aumid-tweakers/blob/main/LICENSE).

Disclaimer: The codes stored herein are my own personal codes and do not related my employer's any way.
