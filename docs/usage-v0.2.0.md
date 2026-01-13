# Usage of the AppUserModelID Stopgap Tools v0.2.0

## Using tools

The AppUserModelID Stopgap Tools consist of two tools. You need use both tools together to work as expected.

- **mklnkwaumid** - The tool for creates special shortcut file (`.lnk`). That shortcut file has a specified AppUserModelID.
- **runwaumid** - The tool for launch an application with a specified AppUserModelID.

### mklnkwaumid

The **mklnkwaumid** creates a shortcut file (`.lnk`) that has a specified AppUserModelID. See [Typical use cases](#typical-use-cases) for specific usage.

```powershell
mklnkwaumid <ShortcutFilePath> <AppUserModelID> <Target> [<ParametersForTarget>]
```

- `ShortcutFilePath`: The file path of the shortcut file to create. The shortcut file has `.lnk` as an extension.
- `AppUserModelID`: The AppUserModelID to set to the shortcut file. See [AppUserModelID form](#appusermodelid-form) for details.
- `Target`: The target of the shortcut file. Usually, it's an executable file that you want to launch from the shortcut you created.
- `ParametersForTarget` (optional): The parameters for the target.

### runwaumid

The **runwaumid** executes an application then set a specified AppUserModelID to the main window of the launched application. See [Typical use cases](#typical-use-cases) for specific usage.

The **runwaumid** provides three different window finding modes to set AppUserModelID.

#### Find the target window by the partial window title text

This mode is useful for apps that work with multi-process architecture such as Electron apps. Because the process ID of the process that first launched process and actual app's process ID are not the same.

```powershell
runwaumid -tp[:<TimeoutInMilliseconds>] <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-tp`: The window title text partial matching mode. Use the text specified as `WindowTitle` to identify the target window.
- `TimeoutInMilliseconds` (optional): The timeout in milliseconds to allow find the target window. If couldn't find the target window in this timeout period, the runwaumid don't apply the specified AppUserModelID and exit. This option useful if the target application takes long time to launch. The default timeout is 5000.
- `AppUserModelID`: The AppUserModelID to set to the target window (the found window). The windows that have the same AppUserModelID are grouped on the taskbar. See [AppUserModelID form](#appusermodelid-form) for details.
- `WindowTitle`: The partial window title text to use to find the target window.
- `TargetToOpen`: The target to open by shell such as an executable file (`.exe`), a document file, an address, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

#### Find the target window by the exact window title text

This mode is useful for apps that work with multi-process architecture such as Electron apps. Because the process ID of the process that first launched process and actual app's process ID are not the same.

```powershell
runwaumid -te[:<TimeoutInMilliseconds>] <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-te`: The window title text exact matching mode. Use the text specified as `WindowTitle` to identify the target window.
- `TimeoutInMilliseconds` (optional): The timeout in milliseconds to allow find the target window. If couldn't find the target window in this timeout period, the runwaumid don't apply the specified AppUserModelID and exit. This option useful if the target application takes long time to launch. The default timeout is 5000.
- `AppUserModelID`: The AppUserModelID to set to the target window (the found window). The windows that have the same AppUserModelID are grouped on the taskbar. See [AppUserModelID form](#appusermodelid-form) for details.
- `WindowTitle`: The exact window title text to use to find the target window.
- `TargetToOpen`: The target to open by shell such as an executable file (`.exe`), a document file, an address, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

#### Find the target window by the process ID

This mode is useful for apps that work with single-process. This mode does not depend on the app's window title text to find the target window.

```powershell
runwaumid -p[:<TimeoutInMilliseconds>] <AppUserModelID> <TargetToOpen> [<ParametersForTargetToOpen>]
```

- `-p`: The process ID mode. Use the process ID of the launched process to identify the target window.
- `TimeoutInMilliseconds` (optional): The timeout in milliseconds to allow find the target window. If couldn't find the target window in this timeout period, the runwaumid don't apply the specified AppUserModelID and exit. This option useful if the target application takes long time to launch. The default timeout is 5000.
- `AppUserModelID`: The AppUserModelID to set to the target window (the found window). The windows that have the same AppUserModelID are grouped on the taskbar. See [AppUserModelID form](#appusermodelid-form) for details.
- `TargetToOpen`: The target to open by shell such as an executable file (`.exe`), a document file, an address, etc.
- `ParametersForTargetToOpen` (optional): The parameters for the target to open by shell.

## AppUserModelID form

AppUserModelIDs should have the following form. It can have no more than 128 characters and cannot contain spaces. Each section should be pascal-cased. `CompanyName` and `ProductName` should always be used, while the `SubProduct` and `VersionInformation` portions are optional. See [How to Form an Application-Defined AppUserModelID](https://learn.microsoft.com/en-us/windows/win32/shell/appids#how-to-form-an-application-defined-appusermodelid) to more details.

```
CompanyName.ProductName[.SubProduct[.VersionInformation]]
```

## Typical use cases

Typically, you need just two steps.

1. Create a shortcut file (.lnk) using the **mklnkwaumid** to execute the target application with parameters via the **runwaumid**.
2. Drag and drop the shortcut file that was created on the first step to the taskbar.

### Example 1: Obsidian with a different vault

1. Create a shortcut file (.lnk) for each vault using the **mklnkwaumid**. For example, execute the following command in ***Windows* PowerShell** such as Windows PowerShell 5.1 to create a shortcut file.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault1.lnk' Tksh164.Obsidian.Vault1 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault1 \"Vault1 - Obsidian\" obsidian://open?vault=Vault1'
    ```

    If you use **PowerShell** (*non-Windows* PowerShell such as PowerShell 7.x), execute the following command instead. There is difference on escape double quote.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault1.lnk' Tksh164.Obsidian.Vault1 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault1 "Vault1 - Obsidian" obsidian://open?vault=Vault1'
    ```

    - The **mklnkwaumid** creates `D:\temp\Obsidian-Vault1.lnk` file and sets `Tksh164.Obsidian.Vault1` to the shortcut file as an AppUserModelID. You can specify your own AppUserModelID, but it's should be follow the [AppUserModelID form](#appusermodelid-form). Also, it should be unique for each group on the taskbar.
    - The `D:\temp\Obsidian-Vault1.lnk` will launches the **runwaumid** with parameters.
    - The **runwaumid** executes `obsidian://open?vault=Vault1` then find the window that has `Vault1 - Obsidian` partially in the window's title text. After found the window, the **runwaumid** sets `Tksh164.Obsidian.Vault1` to the window as an AppUserModelID.
    - It is important that specify the same AppUserModelID both the shortcut (created by **mklnkwaumid**) and the window (executed via **runwaumid**) for grouping the pinned icon and windows.

    You can create multiple shortcut files in the same way. For example:

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault2.lnk' Tksh164.Obsidian.Vault2 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault2 \"Vault2 - Obsidian\" obsidian://open?vault=Vault2'
    ```

    **Note:** `obsidian://open?vault=Vault1` and `obsidian://open?vault=Vault2` in the above examples are the URI to open Obsidian vaults. This is a capability of Obsidian. The URI supports [some formats](https://help.obsidian.md/Extending+Obsidian/Obsidian+URI#Examples). In the above examples, specified the vault name such as `Vault1` and `Vault2` to open a vault. Also, you can specify a vault ID to open an Obsidian vault. Your Obsidian vault's ID can copy from the vaults management window in Obsidian.

    ![Copy the Obsidian vault's ID from the vaults management window in Obsidian](media/obsidian-vault-id-01.png)

    If you want to use a vault ID to open your Obsidian vault, execute the **mklnkwaumid** like the following:

    **With *Windows* PowerShell such as Windows PowerShell 5.1:**

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault1.lnk' Tksh164.Obsidian.Vault1 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault1 \"Vault1 - Obsidian\" obsidian://open?vault=ef6ca3e3b524d22f'
    ```

    **With PowerShell (*non-Windows* PowerShell such as PowerShell 7.x):**

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\Obsidian-Vault1.lnk' Tksh164.Obsidian.Vault1 'C:\bin\runwaumid.exe' '-tp Tksh164.Obsidian.Vault1 "Vault1 - Obsidian" obsidian://open?vault=ef6ca3e3b524d22f'
    ```

2. Customize the created shortcut files (e.g. `D:\temp\Obsidian-Vault1.lnk`) as you like. For instance, Icon, Comment, etc.

3. Drag and drop the shortcut files (e.g. `D:\temp\Obsidian-Vault1.lnk`) on the taskbar. You can delete the shortcut files after the drag and drop those on the taskbar.

### Example 2: Visual Studio Code with a different workspace

1. Create a shortcut file (.lnk) for each workspace using the **mklnkwaumid**. For example, execute the following command in ***Windows* PowerShell** such as Windows PowerShell 5.1 to create a shortcut file.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\VSCode-Workspace1.lnk' Tksh164.VSCode.Workspace1 'C:\bin\runwaumid.exe' '-tp Tksh164.VSCode.Workspace1 \"Workspace1 (Workspace) - Visual Studio Code\" \"C:\Program Files\Microsoft VS Code\Code.exe\" \"D:\VSCode\Workspace1.code-workspace\"'
    ```

    If you use **PowerShell** (*non-Windows* PowerShell such as PowerShell 7.x), execute the following command instead. There is difference on escape double quote.

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\VSCode-Workspace1.lnk' Tksh164.VSCode.Workspace1 'C:\bin\runwaumid.exe' '-tp Tksh164.VSCode.Workspace1 "Workspace1 (Workspace) - Visual Studio Code" "C:\Program Files\Microsoft VS Code\Code.exe" "D:\VSCode\Workspace1.code-workspace"'
    ```

    - The **mklnkwaumid** creates `D:\temp\VSCode-Workspace1.lnk` file and sets `Tksh164.VSCode.Workspace1` to the shortcut file as an AppUserModelID. You can specify your own AppUserModelID, but it's should be follow the [AppUserModelID form](#appusermodelid-form). Also, it should be unique for each group on the taskbar.
    - The `D:\temp\VSCode-Workspace1.lnk` will launches the **runwaumid** with parameters.
    - The **runwaumid** executes `"C:\Program Files\Microsoft VS Code\Code.exe" "D:\VSCode\Workspace1.code-workspace"` then find the window that has `Workspace1 (Workspace) - Visual Studio Code` partially in the window's title text. After found the window, the **runwaumid** sets `Tksh164.VSCode.Workspace1` to the window as an AppUserModelID.
    - It is important that specify the same AppUserModelID both the shortcut (created by **mklnkwaumid**) and the window (executed via **runwaumid**) for grouping the pinned icon and windows.

    You can create multiple shortcut files in the same way. For example:

    ```powershell
    .\mklnkwaumid.exe 'D:\temp\VSCode-Workspace2.lnk' Tksh164.VSCode.Workspace2 'C:\bin\runwaumid.exe' '-tp Tksh164.VSCode.Workspace2 \"Workspace2 (Workspace) - Visual Studio Code\" \"C:\Program Files\Microsoft VS Code\Code.exe\" \"D:\VSCode\Workspace2.code-workspace\"'
    ```

2. Customize the created shortcut file (e.g. `D:\temp\VSCode-Workspace1.lnk`) as you like. For instance, Icon, Comment, etc.

3. Drag and drop the shortcut file (e.g. `D:\temp\VSCode-Workspace1.lnk`) on the taskbar. You can delete the shortcut files after the drag and drop those on the taskbar.

### Example 3: Eclipse IDE with a different workspace

The Eclipse IDE takes long time to launch most cases. In this case, specify `TimeoutInMilliseconds` to get an expected result.

1. Create a shortcut file (.lnk) for each workspace using the **mklnkwaumid**. For example, execute the following command in ***Windows* PowerShell** such as Windows PowerShell 5.1 to create a shortcut file.

    ```powershell
    .\mklnkwaumid.exe 'C:\temp\Eclipse-Workspace1.lnk' YourOwn.AppUserModelID.Workspace1 'C:\bin\runwaumid.exe' '-tp:30000 YourOwn.AppUserModelID.Workspace1 \"Workspace1 - Eclipse IDE\" \"C:\Users\User1\eclipse\java-2025-12\eclipse\eclipse.exe\" \"-data \"\"C:\Users\User1\Workspace1\"\"\"'
    ```

    If you use **PowerShell** (*non-Windows* PowerShell such as PowerShell 7.x), execute the following command instead. There is difference on escape double quote.

    ```powershell
    .\mklnkwaumid.exe 'C:\temp\Eclipse-Workspace1.lnk' YourOwn.AppUserModelID.Workspace1 'C:\bin\runwaumid.exe' '-tp:30000 YourOwn.AppUserModelID.Workspace1 "Workspace1 - Eclipse IDE" "C:\Users\User1\eclipse\java-2025-12\eclipse\eclipse.exe" "-data ""C:\Users\User1\Workspace1"""'
    ```

    - The **mklnkwaumid** creates `C:\temp\Eclipse-Workspace1.lnk` file and sets `YourOwn.AppUserModelID.Workspace1` to the shortcut file as an AppUserModelID. You can specify your own AppUserModelID, but it's should be follow the [AppUserModelID form](#appusermodelid-form). Also, it should be unique for each group on the taskbar.
    - The `C:\temp\Eclipse-Workspace1.lnk` will launches the **runwaumid** with parameters.
    - The **runwaumid** executes `"C:\Users\User1\eclipse\java-2025-12\eclipse\eclipse.exe" -data "C:\Users\User1\Workspace1"` then find the window that has `Workspace1 - Eclipse IDE` partially in the window's title text with a timeout of `30000` milliseconds. After found the window, the **runwaumid** sets `YourOwn.AppUserModelID.Workspace1` to the window as an AppUserModelID.
    - It is important that specify the same AppUserModelID both the shortcut (created by **mklnkwaumid**) and the window (executed via **runwaumid**) for grouping the pinned icon and windows.

    You can create multiple shortcut files in the same way.

2. Customize the created shortcut file (e.g. `C:\temp\Eclipse-Workspace1.lnk`) as you like. For instance, Icon, Comment, etc.

3. Drag and drop the shortcut file (e.g. `C:\temp\Eclipse-Workspace1.lnk`) on the taskbar. You can delete the shortcut files after the drag and drop those on the taskbar.
