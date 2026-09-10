# FlyThings Development Documentation (Simplified)

## Table of Contents

1. [About Us](#about-us)
2. [Installation](#installation)
   - Installation environment
   - First use
   - Interface overview
3. [Development Workflow](#development-workflow)
   - Creating a project
   - Project structure
   - Building the project
   - Creating a UI file
   - Running the project
   - UI files and generated code
   - Handy tips for the development tools
   - Importing a project
4. [Manifest](#manifest)
   - Manifest introduction
   - Switching platforms
   - Adding dependency packages
   - Adding local libraries
   - Building a static or dynamic library
5. [UI Interaction](#ui-interaction)
   - Startup screen
   - Opening/closing screens
   - Screen lifecycle
6. [Timers](#timers)
7. [Serial Communication](#serial-communication)
   - Linux serial programming
   - Introduction / the communication framework
   - Serial configuration / multi-port configuration
8. [Network Control](#network-control)
   - Wi-Fi settings / hotspot settings
9. [System Operations](#system-operations)
    - Data storage / system time / TF card and USB drive / TF card insertion events
    - GPIO operations / SPI operations / rebooting the system / memory and CPU information
10. [Upgrading and Debugging](#upgrading-and-debugging)
    - ADB debugging / viewing logs / TF card boot
    - Building an upgrade image / automatic upgrade / remote upgrade / batch upgrade
11. [Appendix](#appendix)
    - FAQ / installing the ADB driver

---

# About Us

## Goal
To make development simpler and more efficient, and to give ordinary users an easier path to customized development.

## Values
- Professional: building professional products with professional technology
- Innovative: continuously innovating to deliver a steady stream of value to customers
- Responsible: changing life through technology

---

# Installation

## Installation Environment

### FlyThings IDE

<a href="https://download.s21i.co99.net/14731609/0/0/ABUIABBPGAAglMLczgYo0Mjk3AU.zip?f=flythings-ide-win32-win32-x86-zkswe-setup.zip&v=1775706403"><strong>FlyThings IDE</strong></a> (20260403)

### Linux Toolchain

For the `SSD`, `Z20`, `Z21` and `Z261` platforms:

* [ssd.tar.gz](https://download.flythings.cn/toolchain/ssd.tar.gz)

---

## First Use

After the installation completes, find the ![ZKSW-Editor shortcut](resources/ide/quick_link.png) shortcut on the desktop and double-click it.  
You can also run ![ide](resources/ide/ide.png) directly from the `bin` folder of the installation directory.

### Choosing a Workspace

On startup, the workspace selection dialog appears. The **workspace** stores the related settings and history, and can manage several projects at once.

![Choosing a workspace](resources/ide/select_workspace.png)

### Welcome Screen

The first time you open the IDE, or after selecting a new workspace, the welcome screen appears with two shortcuts:

- **New project**: guides you step by step through creating a FlyThings project
- **Import project**: imports an existing project into the current workspace so you can continue working on it

![Welcome screen](resources/ide/welcome.png)

---

## Interface Overview

### Project Explorer

Shows the resource files, code files and so on inside the project folder as a tree. It can be expanded/collapsed freely, and double-clicking a file opens it for editing.

![Expanding/collapsing the project](resources/ide/project_open_collapsed.gif)

### Console

When you build the code, the build log is printed here. If the build fails, double-clicking the **error message** jumps straight to the corresponding code.

![Demonstration of double-clicking an error message to jump to the code](resources/ide/usage_console.gif)

---

# Development Workflow

## Creating a Project

1. In the menu bar at the top of the editor, choose **File -> New -> FlyThings Project**.

   ![New project](resources/ide/new_flythings_project.gif)

2. After the previous step, the **FlyThings creation wizard** dialog appears.

   ![Creation wizard step one](resources/ide/wizard_new_project_page1.png)

   Fill in the parameters for the new project as required.

   * **Platform type**: choose the platform matching the serial display you purchased.

3. After clicking Next, more parameters are shown.

   ![New project parameters](resources/ide/wizard_new_project_page2.png)

   * **Screensaver timeout**: if there is no touch input within the specified time, the system enters the screensaver automatically. A time of **-1** seconds disables the screensaver.
   * **Serial port**: the communication port; usually does not need to be changed.
   * **Baud rate**: the baud rate of the communication port.
   * **Resolution**: the width and height of the screen in pixels.
   * **Screen rotation**: for screens with a different axis orientation, tick this to rotate the displayed content by 90°.
   * **Font**: custom fonts are supported.
   * **Input method**: must be ticked if you need to enter Chinese; used together with the editable input field widget.

   All of the above properties can be changed again later.

4. Click Next again to set the project name and save location.

   ![New project step three](resources/ide/wizard_new_project_page3.png)

   * **Project name**: a combination of letters and digits; no Chinese characters or spaces.
   * **Location**: the project's storage path; Chinese characters in the path are not recommended.

   When you are done, click **Finish** to complete the creation.

---

## Creating a UI File

1. In the project explorer, expand the project, select the `ui` folder, right-click and choose `New` -> `FlyThings UI File`.

   ![](resources/ide/new_ftu.gif)

2. Fill in the following parameters in the wizard dialog:
   - **File name**: named with letters and digits, with the extension `ftu`.
   - **Resolution**: controls the width and height of the UI screen, in pixels.
   - **Window type**: usually `Normal`; the other types (status bar, navigation bar, screensaver) are system screens.

   ![New UI file wizard](resources/ide/wizard_new_ftu.png)

   After confirming the parameters, click **Finish**; the new UI file appears in the project's `ui` folder.

---

## Project Structure

![Project structure](resources/project_structure.png)

A default project usually has the three folders `src`, `resources` and `ui`, plus a `Manifest.xml` file.

### The ui Folder

![The ui folder expanded](resources/project_ui_expand.png)

Expanding the `ui` folder shows the `main.ftu` file created by default. `ftu` is the extension of UI files, and every `ftu` file corresponds to one application screen.

### The resources Folder

Everything in this folder is packaged together with the program; it is mainly used for resource files such as images.

In code you can get the absolute path of a file under `resources`:

```c++
#include <manager/ConfigManager.h>

std::string test_txt_path = CONFIGMANAGER->getResFilePath("test.txt");
std::string back_png_path = CONFIGMANAGER->getResFilePath("sub/back.png");
```

### The src Folder

Mainly holds the source files.

![](resources/project_jni_expand.png)

It usually contains `activity`, `logic`, `uart` and `Main.cpp`.

#### The activity Folder
An automatically generated directory; each UI file produces a corresponding `Activity` class file when built.

> **⚠️ Note**: do not modify the automatically generated code under the `activity` folder by hand.

#### The logic Folder
Each UI file produces a `Logic.cc` file with the matching prefix when built; the screen's events and control logic go here.

#### The uart Folder
Holds the code related to serial port operations, including reading from and writing to the serial port, protocol parsing and so on.

#### Main.cpp
The entry point of the whole application, including choosing the startup screen and initialization.

```c++
#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"
#ifdef __cplusplus
extern "C" {
#endif

void onEasyUIInit(EasyUIContext *pContext) {
    UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), CONFIGMANAGER->getUartBaudRate());
}

void onEasyUIDeinit(EasyUIContext *pContext) {
    UARTCONTEXT->closeUart();
}

const char* onStartupApp(EasyUIContext *pContext) {
    return "mainActivity";
}

#ifdef __cplusplus
}
#endif
```

### The Manifest File
The project's manifest file; editing it lets you change the platform, add or remove dependency packages and more. See the [Manifest chapter](#manifest) for details.

---

## UI Files and Generated Code

FlyThings keeps the UI and the code separate, which makes them easier to manage. Before building, the tool generates a `Logic.cc` file with the same prefix from each UI file (incrementally — it is never fully overwritten). The tool walks through each UI file and declares a pointer variable for each widget it contains, defined in the `Activity.cpp` file with the same prefix.

![](resources/global_control_pointer.png)

As the image shows, all pointers are static global variables, and `mainActivity.cpp` contains the statement `#include "logic/mainLogic.cc"`, so these widget pointers can be used directly in `mainLogic.cc`.

### Functions Generated Automatically in Logic.cc

| Function | Description |
|------|------|
| `REGISTER_ACTIVITY_TIMER_TAB[]` | Registers timers; enter the timer id and the interval in milliseconds |
| `onUI_init()` | Called when the screen is initialized; corresponds to `onCreate()` |
| `onUI_intent()` | Fires when switching to this screen |
| `onUI_show()` | Fires when the screen is shown |
| `onUI_hide()` | Fires when the screen is hidden |
| `onUI_quit()` | Called when the screen exits; corresponds to the destructor |
| `onProtocolDataUpdate()` | The serial data callback; fires when a parsed data frame is received |
| `onUI_Timer(int id)` | The timer callback; return `true` to keep running, `false` to stop |

---

## Building the Project

There are three places from which you can build.

### Building from the Project Explorer
In the project explorer, right-click the project name and choose `Build FlyThings` from the context menu.

![Choosing Build from the context menu](resources/compile_from_context_menu.gif)

### Building from the Toolbar
Left-click to select the project name, then click the green triangle button ![](resources/compile_from_toolbar.png) on the toolbar to build.

### Build Shortcut
The shortcut is `Ctrl + Alt + Z`; this is the recommended way.

### Cleaning the Project
Building automatically creates a `Release` folder for temporary files, which you may delete at any time. You can also right-click the project name and choose `Clean project` to clear the cached files.

![Automatically deleting the folder](resources/delete_folder.gif)

### Rebuilding
Clean the project first, then build again for a complete rebuild.

---

## Running the Project

Once the project has been built, you can get it onto real hardware in the following ways:

### Running Quickly over a Wi-Fi Connection
Only supported on models with Wi-Fi.

1. Open the device's Wi-Fi settings screen and connect the device to the same wireless network as your computer.
2. Once connected, look up the device's IP address.
3. In the development tool's menu bar, choose **Debug configuration** -> **ADB configuration**, set the ADB connection type to **WIFI**, enter the device IP, choose the download directory and save.
4. Choose the **Download and debug** menu item to sync the project code to the device and run it.

### Running Quickly over a USB Connection
For models without Wi-Fi. **Note: on models with Wi-Fi the USB connection does not work.**

1. Connect the device to the computer with a USB cable; if you get a driver prompt, see the section on installing the ADB driver.
2. In the menu bar, choose **Debug configuration** -> **ADB configuration**, set the ADB connection type to **USB**, choose the download directory and save.
3. Choose the **Download and debug** menu item to sync the project code to the device and run it.

### Booting from a TF Card
If neither USB nor Wi-Fi is available, you can boot the program from a TF card; for the exact steps see [Booting the program from a TF card](#upgrading-and-debugging).

---

## Handy Tips for the Development Tools

### Jumping Quickly to the Associated Function
Select a widget in the preview, right-click and choose **Jump to related code** to open the corresponding Logic.cc file at the associated function.

![](resources/ide/jump_to_source.png)

### Changing the Font / Font Size
In the menu bar choose **Window** -> **Preferences** and configure the font in the dialog.

![](resources/ide/preference.png)
![](resources/ide/set_font.png)

### Code Completion
While editing code, type the beginning of a name and press **Alt + /** to open code completion.

![](resources/ide/intelisence.png)

### Rebuilding the Index
If code completion stops working, or the editor reports errors while the build succeeds, try rebuilding the index.

![](resources/ide/rebuild_index.gif)

---

## Importing a Project

1. In the menu bar choose **File** -> **Import**.
2. In the dialog choose **General** -> **Existing Projects into Workspace** and click **Next**.
3. Click the **Browse** button, select the folder of the project to import and click **OK**.
4. Confirm the detected project and click **Finish**; the project appears in the project explorer.

![](resources/ide/import_project.gif)

---

# Manifest

## Manifest Introduction

`Manifest.xml` is an important configuration file; editing it lets you configure the project platform, manage dependency packages and more.

Double-click `Manifest.xml` to edit it.

![](resources/manifest.png)

---

## Switching Platforms

Double-click `Manifest.xml`, switch the platform directly in the project platform dropdown, and press `Ctrl+S` to save the change.

![](resources/ide/switch_platform.png)

The development tool generates the corresponding macro definitions for the current platform, so the code can check which platform it is running on:

```c++
#if __PLATFORM_Z6S__
// Z6S platform
#elif __PLATFORM_Z21__
// Z21 platform
#elif __PLATFORM_Z20__
// Z20 platform
#elif __PLATFORM_Z261__
// Z261 platform
#elif __PLATFORM_T113__
// T113 platform
#elif __PLATFORM_T113EMMC__
// T113EMMC platform
#elif __PLATFORM_V85X__
// V85X platform
#endif
```

When a project has to run on several platforms, conditional compilation based on these macros is recommended, so that only one copy of the code has to be maintained.

---

## Adding Dependency Packages

Double-click `Manifest.xml` to start editing the configuration.

When code is built into a static/dynamic library, packaged together with its header files, given a version number and uploaded to the server, it is called a `dependency package`. You can browse the existing packages in the <a href="https://package.flythings.cn/" target="_blank">dependency package repository</a>.

Click `Add`, enter the package name and version number in the dialog, click OK and press `Ctrl+S` to save.

![](resources/ide/add_pkg.png)

It is recommended to click `Copy` in the dependency package repository and then paste with `Ctrl+V` into the input field, which fills in the package name and version number automatically.

![](resources/base_pkg.png)

> **⚠️ Important**: after adding it, remember to click the `Update dependencies` menu item to complete the update.

![](resources/ide/update_dep.png)

Once the update succeeds, the dependency package's header files appear under the project's `Include`, and can be referenced in the code:

```c++
#include <base/base.h>
```

![](resources/ide/add_pkg_ok.png)

---

## Adding Local Libraries

For private static/dynamic libraries, you can put the library files into an agreed folder and they will be linked automatically at build time.

1. Create a `dependencies` folder inside the project's `src` folder.
2. Create an `include` folder inside `dependencies` (used as a header search directory at build time).
3. Create a `lib` folder inside `dependencies` (all static and dynamic libraries in it are linked automatically at build time).
   - If a `dependencies/lib-no-link` folder exists, the dynamic libraries in it are only packaged with the program and are not part of the build.
   - Library file names must start with `lib`, with the extension `.a` for static libraries and `.so` for dynamic libraries; otherwise they are ignored.
4. Copy the header files into the `include` folder and the library files into the `lib` folder, giving the following structure:

   ![](resources/local_dependencies.png)

5. Right-click the project name and choose `Update dependencies` from the context menu; the dependencies take effect after the next build.
6. Reference them in the code:
   ```c++
   #include "foo.h"
   #include "abc.h"
   ```

---

## Building a Static or Dynamic Library

Double-click `Manifest.xml` to open it, switch to the `Source files` view, and add `target` tags according to the rules to build a static or dynamic library.

![](resources/ide/manifest_source_tab.png)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xml>
<manifest platform="Z21">
    <targets>
        <!-- Produces the static library libfoo1.a -->
        <target id="foo1" type="staticLibrary">
            <header filter="src/foo/hello.h" />
            <source filter="src/foo/hello.cpp" />
        </target>

        <!-- Produces the dynamic library libfoo2.so -->
        <target id="foo2" type="sharedLibrary">
            <header filter="src/foo/hello.h" />
            <source filter="src/foo/hello.cpp" />
        </target>
    </targets>
</manifest>
```

- `id`: the library name; `type`: `staticLibrary` or `sharedLibrary`
- `header`: the header files shipped with the library; `filter` supports wildcards
- `source`: the source files to compile into the library; `filter` supports wildcards

After the build, the library files are produced in the `Release` directory.

![](resources/manifest_target.png)

---

# UI Interaction

## Startup Screen

In the `Main.cpp` generated when the project is created, the return value of the `onStartupApp()` function determines the startup screen:

```c++
const char* onStartupApp(EasyUIContext *pContext) {
    return "mainActivity";  // corresponds to main.ftu
}
```

## Opening/Closing Screens

### Opening a Screen

```c++
EASYUICONTEXT->openActivity("subActivity");
```

To pass parameters:

```c++
Intent *pIntent = new Intent();
pIntent->putExtra("cmd", "open");
pIntent->putExtra("value", "ok");
EASYUICONTEXT->openActivity("subActivity", pIntent);
```

Receive them in the target screen's `onUI_intent`:

```c++
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr) {
        std::string cmd = intentPtr->getExtra("cmd");   // "open"
        std::string value = intentPtr->getExtra("value"); // "ok"
    }
}
```

> An Intent created with new does not have to be deleted manually; the framework releases it internally.

### Closing a Screen

```c++
EASYUICONTEXT->goBack();       // Return to the previous screen
EASYUICONTEXT->goHome();       // Go straight back to the startup screen
EASYUICONTEXT->closeActivity("subActivity"); // Close a specific screen (the startup screen cannot be closed)
```

You can also set a button's ID to `sys_back` or `sys_home`, and the system will perform the corresponding action automatically.

---

## Screen Lifecycle

Screens form a **stack**, with the most recently opened screen on top.

### The Flow When Opening a Screen

- If the screen is **not** on the stack: `onUI_init` → `onUI_intent` → `onUI_show` fire in that order
- If the screen is **already** on the stack: it is only moved to the top, and `onUI_init` does not fire

### The Flow When Closing a Screen

- `goBack()`: pops the top screen, fires its `onUI_quit`, then fires `onUI_show` on the screen below
- `goHome()`: goes straight back to the startup screen, popping all other screens
- `closeActivity("xxx")`: removes the specified screen; if the removed screen is not the top one, `onUI_show` does not fire on the one below

> If a screen allocates resources when it opens, remember to release them in `onUI_quit`.

---

# Timers

## Timers

Timers suit any scenario that needs an operation performed on a schedule, for example sending heartbeat packets, querying data and refreshing the screen periodically, or polling tasks.

### Registering a Timer

Add a struct to the timer array in `Logic.cc` to register a timer:

```c++
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
    {0, 6000},  // timer id=0, interval 6 seconds
    {1, 1000},  // timer id=1, interval 1 second
};
```

The struct definition:

```c++
typedef struct {
    int id;   // Timer ID, must be unique
    int time; // Interval, in milliseconds
} S_ACTIVITY_TIMEER;
```

### The Timer Callback

When a timer fires, the system calls `onUI_Timer(int id)` in the corresponding `Logic.cc`:

```c++
static bool onUI_Timer(int id) {
    switch (id) {
        case 0:
            // Logic for the timer with id=0
            break;
        case 1:
            // Logic for the timer with id=1
            break;
        default:
            break;
    }
    return true;  // Return true to keep running, false to stop
}
```

> **📌 Note**
> - Each screen's timers are independent, so timer ids may be reused across screens
> - Timers keep running while the screen has not been destroyed, and stop automatically once it is

---

## Registering/Stopping Timers Manually

The `REGISTER_ACTIVITY_TIMER_TAB` approach is not flexible enough, so the Activity class provides the following three methods for controlling timers dynamically:

```c++
void registerUserTimer(int id, int time);   // Register a timer
void unregisterUserTimer(int id);            // Cancel a timer
void resetUserTimer(int id, int time);       // Reset a timer
```

Usage example:

```c++
static bool isRegistered = false;
#define TIMER_HANDLE 2

static bool onButtonClick_ButtonTimerOn(ZKButton *pButton) {
    if (!isRegistered) {
        mActivityPtr->registerUserTimer(TIMER_HANDLE, 500);
        isRegistered = true;
    }
    return false;
}

static bool onButtonClick_ButtonTimerOff(ZKButton *pButton) {
    if (isRegistered) {
        mActivityPtr->unregisterUserTimer(TIMER_HANDLE);
        isRegistered = false;
    }
    return false;
}
```

> **⚠️ Important**: the three functions `registerUserTimer`, `unregisterUserTimer` and `resetUserTimer` must **not** be called inside `onUI_Timer`, as that causes a deadlock.

---

# Serial Communication

## Linux Serial Programming

> **This section is meant to help you understand how the serial part of a FlyThings project is built up from scratch, which makes the final serial code flow easier to follow.**

This product is based on Linux, so the serial port can be operated entirely with standard Linux programming, in the following 5 steps:

### 1. Opening the Serial Port

```c++
#include <fcntl.h>

int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
```

`/dev/ttyS0` is similar to `COM1` on Windows. On success a non-negative serial descriptor is returned; on failure a negative number.

### 2. Configuring the Serial Port

```c++
int openUart() {
    int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
    struct termios oldtio = { 0 };
    struct termios newtio = { 0 };
    tcgetattr(fd, &oldtio);
    newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = 0;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}
```

> **Note**: the above is the default configuration (8 data bits, 1 stop bit, no parity); do not change it unless you have a special requirement.

### 3. Reading from the Serial Port

```c++
unsigned char buffer[1024] = {0};
int ret = read(fd, buffer, sizeof(buffer));
```

A return value > 0 means data was read and gives the number of bytes; ≤ 0 means an error or no data available yet.

> **Note**: `read` does not guarantee that all data is read at once, so it has to be called repeatedly to make sure the data is complete.

### 4. Writing to the Serial Port

```c++
unsigned char buffer[4] = {0x01, 0x02, 0x03, 0x04};
int ret = write(fd, buffer, sizeof(buffer));
```

A return value equal to the third argument means the data was sent successfully; anything else indicates a problem.

### 5. Closing the Serial Port

```c++
close(fd);
```

---

## Serial Port Introduction

The display and the MCU communicate over a serial port; as long as both sides agree on a protocol they can interact. Unlike traditional serial displays, which act as slaves, the FlyThings display has processing logic of its own and acts as the master.

When a project is created, the tool generates the serial communication code automatically and provides callback interfaces between the protocol data and the UI, so developers only need to care about presenting the data on the UI while the framework handles the communication. The protocol parsing part has to be adapted to the actual communication protocol.

---

## The Communication Framework

### Code Structure

The software app is split into two layers:

**The serial HAL layer**
- `UartContext`: the serial entity control layer, providing interfaces to open/close the port and to send and receive
- `ProtocolData`: defines the communication data struct, holding the variables parsed out of the protocol
- `ProtocolSender`: wraps up sending data
- `ProtocolParser`: parses the protocol, stores the data in `ProtocolData` and manages the callback interfaces for serial data changes

**The app interface layer**
- Registers a listener with `ProtocolParser` to get the updated `ProtocolData`
- Sends commands to the MCU through `ProtocolSender`

`UartContext` is a standard implementation and generally does not need to be modified.

### Protocol Format

Taking a common protocol as an example:

| Header (2 bytes) | Command (2 bytes) | Data length (1 byte) | Data (N bytes) | Checksum (1 byte, optional) |
|:-----------:|:---------:|:------------:|:-----------:|:---------------:|
| 0xFF55 | Cmd | len | data | checksum |

The frame header and the minimum packet length are defined in `CommDef.h`:

```c++
// To enable the checksum: #define PRO_SUPPORT_CHECK_SUM
#define CMD_HEAD1   0xFF
#define CMD_HEAD2   0x55
#define DATA_PACKAGE_MIN_LEN  5  // Without checksum: 2+2+1=5; with checksum: +1=6
```

#### Changing the Protocol Header

```c++
// 1. Change the frame header definition
#define CMD_HEAD1   0xFF
#define CMD_HEAD2   0x55

// 2. Update the check accordingly when the header length changes
while ((mDataBufLen >= 2) && ((pData[0] != CMD_HEAD1) || (pData[1] != CMD_HEAD2)))
```

#### Changing the Position of the Length Field

```c++
dataLen = pData[4];                        // pData[4] is the length byte; change as needed
frameLen = dataLen + DATA_PACKAGE_MIN_LEN; // Frame length = data length + header/trailer length
```

---

## Serial Port Configuration

### Mapping of Serial Port Numbers

| Platform | Software port | Hardware port |
|:---:|:--------:|:--------:|
| Z11 series | ttyS0 / ttyS1 | UART1 / UART2 |
| Z6 series | ttyS0 / ttyS1 / ttyS2 | UART0 / UART1 / UART2 |
| Z20 series | ttyS1 / ttyS2 / ttyS3 | UART1 / FUART / UART2 |
| Z21 series | ttyS1 / ttyS2 / ttyS3 | UART1 / UART2 / UART3 |

### Configuring the Baud Rate

The baud rate can be set in the wizard when creating a project, or changed later by right-clicking the project and choosing `Properties`.

### Opening and Closing the Serial Port

Controlled in `/Main.cpp`:

```c++
void onEasyUIInit(EasyUIContext *pContext) {
    UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), CONFIGMANAGER->getUartBaudRate());
}

void onEasyUIDeinit(EasyUIContext *pContext) {
    UARTCONTEXT->closeUart();
}
```

---

## Multi-Port Configuration

A normal project supports one serial port by default. For several ports, the serial code has to be modified.

The port numbers and baud rates are configured in `init()` in `/uart/UartContext.cpp`:

```c++
void UartContext::init() {
    uart0 = new UartContext(UART_TTYS0);
    uart0->openUart("/dev/ttyS0", B9600);

    uart1 = new UartContext(UART_TTYS1);
    uart1->openUart("/dev/ttyS1", B9600);
}
```

Sending data to a specific port:

```c++
unsigned char buf[2] = {1, 1};
sendProtocolTo(UART_TTYS1, 1, buf, 2); // Send to ttyS1
sendProtocolTo(UART_TTYS0, 1, buf, 2); // Send to ttyS0
```

To distinguish where the data came from, add an identifying field to `SProtocolData`:

```c++
// ProtocolData.h
typedef struct {
    BYTE power;
    int uart_from; // Which serial port it came from
} SProtocolData;
```

Set it in `procParse`, and read `data.uart_from` in `onProtocolDataUpdate` in `Logic.cc` to tell where it came from.

---

# Network Control

## Wi-Fi Settings

```c++
#include "net/NetManager.h"
```

Opening the built-in Wi-Fi settings screen:

```c++
EASYUICONTEXT->openActivity("WifiSettingActivity");
```

Getting the WifiManager:

```c++
WifiManager *pWM = NETMANAGER->getWifiManager();
// Or define a macro for convenience
#define WIFIMANAGER NETMANAGER->getWifiManager()
```

Common interfaces:

```c++
WIFIMANAGER->isSupported();        // Check whether Wi-Fi is supported
WIFIMANAGER->isWifiEnable();       // Check whether Wi-Fi is enabled
WIFIMANAGER->enableWifi(true);     // Turn Wi-Fi on/off
WIFIMANAGER->startScan();          // Scan for Wi-Fi networks
WIFIMANAGER->connect(ssid, pw);    // Connect to a Wi-Fi network
WIFIMANAGER->disconnect();         // Disconnect
WIFIMANAGER->isConnected();        // Whether it is connected
WIFIMANAGER->getConnectionInfo();  // Get information about the connected network

// Registering/unregistering a listener
void addWifiListener(IWifiListener *pListener);
void removeWifiListener(IWifiListener *pListener);
```

---

## Hotspot Settings

Opening the built-in hotspot settings screen:

```c++
EASYUICONTEXT->openActivity("SoftApSettingActivity");
```

Getting the SoftApManager:

```c++
#include "net/NetManager.h"
#define SOFTAPMANAGER NETMANAGER->getSoftApManager()
```

Common interfaces:

```c++
SOFTAPMANAGER->setEnable(true);             // Turn the hotspot on/off
SOFTAPMANAGER->isEnable();                  // Whether the hotspot is on
SOFTAPMANAGER->getSoftApState();            // Get the current hotspot state
SOFTAPMANAGER->setSsidAndPwd("name", "pw"); // Change the hotspot name and password
SOFTAPMANAGER->getSsid();                   // Get the hotspot name
SOFTAPMANAGER->getPwd();                    // Get the hotspot password

// Hotspot state enum
// E_SOFTAP_DISABLED     Off
// E_SOFTAP_ENABLING     Turning on
// E_SOFTAP_ENABLED      Turned on successfully
// E_SOFTAP_DISABLING    Turning off
// E_SOFTAP_ENABLE_ERROR Failed to turn on

// Registering/unregistering a state listener
void addSoftApStateListener(ISoftApStateListener *pListener);
void removeSoftApStateListener(ISoftApStateListener *pListener);
```

---

# System Operations

## Data Storage

Stores small amounts of data permanently as **key-value pairs** (accounts, passwords, settings and so on), preserved across power cycles.

```c++
#include "storage/StoragePreferences.h"
```

Storage interfaces:

```c++
StoragePreferences::putString("username", "zhang san");
StoragePreferences::putInt("age", 20);
StoragePreferences::putBool("power", true);
StoragePreferences::putFloat("temperature", 30.12);
```

Reading interfaces (the second argument is the default value):

```c++
std::string name = StoragePreferences::getString("username", "");
int age          = StoragePreferences::getInt("age", 0);
bool power       = StoragePreferences::getBool("power", false);
float temp       = StoragePreferences::getFloat("temperature", 0);
```

Deleting/clearing:

```c++
StoragePreferences::remove("username");
StoragePreferences::clear();
```

> **⚠️ Note**: the data is stored as files in flash, so **do not write frequently**, to avoid damaging the flash. This partition is usually no more than a few hundred KB; you can check the size of the `data` partition with `adb shell df`.

---

## System Time

```c++
#include "utils/TimeHelper.h"
```

Getting the current time:

```c++
struct tm *t = TimeHelper::getDateTime();
// t->tm_year + 1900 = the actual year
// t->tm_mon + 1 = the actual month
```

Setting the time:

```c++
struct tm t;
t.tm_year = 2017 - 1900;
t.tm_mon  = 9 - 1;
t.tm_mday = 13;
t.tm_hour = 16;
t.tm_min  = 0;
t.tm_sec  = 0;
TimeHelper::setDateTime(&t);

// Or with a string (format: yyyy-MM-dd HH:mm:ss)
TimeHelper::setDateTime("2017-09-13 16:00:00");
```

---

## TF Card / USB Drive

Paths mounted automatically by the system:
- TF card: `/mnt/extsd`
- USB drive: `/mnt/usb1`, `/mnt/usb2`, `/mnt/usb3`

Checking the mount status:

```c++
#include <os/MountMonitor.h>

MOUNTMONITOR->isMounted("/mnt/extsd");  // Whether the TF card is mounted
MOUNTMONITOR->isMounted("/mnt/usb1");   // Whether the USB drive is mounted
```

The path to a file = the mount directory + the file's own path.

---

## Listening for TF Card Insertion/Removal

```c++
#include "os/MountMonitor.h"

class MyMountListener : public MountMonitor::IMountListener {
public:
    virtual void notify(int what, int status, const char *msg) {
        switch (status) {
        case MountMonitor::E_MOUNT_STATUS_MOUNTED:  // Inserted; msg is the mount path
            LOGD("mount path: %s\n", msg);
            break;
        case MountMonitor::E_MOUNT_STATUS_REMOVE:   // Removed; msg is the unmounted path
            LOGD("remove path: %s\n", msg);
            break;
        }
    }
};

static MyMountListener sMyMountListener;

// Register the listener
MOUNTMONITOR->addMountListener(&sMyMountListener);

// Remove the listener
MOUNTMONITOR->removeMountListener(&sMyMountListener);
```

You can also use the newer interface to watch a specific path (requires the `base-utility` package 9.0.0+):

```c++
#include <base/base.h>

static base::MountNotification mn_extsd("/mnt/extsd", onMyMountEvent); // Watch the TF card
static base::MountNotification mn_usb1("/mnt/usb1", onMyMountEvent);   // Watch the USB drive
// An empty string watches all mount events
static base::MountNotification mn_any("", onMyMountEvent);
```

> **Note**: `MountNotification` instances must be declared `static`, otherwise watching stops once the instance is destroyed.

---

## GPIO Operations

```c++
#include "utils/GpioHelper.h"
```

```c++
// Read a pin's state: returns 1 (high), 0 (low) or -1 (failure)
GpioHelper::input("B_02");

// Set a pin's output: 1 high, 0 low; returns 0 on success, -1 on failure
GpioHelper::output("B_02", 1);
```

Available pins per platform (partial list):

| Platform | Available pins |
|:---:|:------|
| Z11S | `B_02`, `B_03`, `E_20` |
| SV50PB | `PIN7`~`PIN14`, `PIN23`, `PIN24`, `PIN26`, `PIN27` |

---

## SPI Operations

```c++
#include "utils/SpiHelper.h"

uint8_t tx[512], rx[512];

// Parameters: bus number, mode, clock frequency, bit width, LSB first
SpiHelper spi(1, SPI_MODE_0, 50*1000*1000, 8, false);

spi.write(tx, 5);                        // Simplex write
spi.read(rx, 8);                         // Simplex read
spi.halfduplexTransfer(tx, 5, rx, 8);    // Half-duplex transfer
```

---

## Rebooting the System

```c++
#include <unistd.h>
#include <sys/reboot.h>

sync();               // Sync the data to prevent loss
reboot(RB_AUTOBOOT);  // Reboot
```

---

## Memory and CPU Information

Check it from an ADB terminal:

```bash
adb shell cat /proc/meminfo   # Memory information
adb shell cat /proc/cpuinfo   # CPU information
adb shell busybox top         # Live CPU usage
```
Reading the memory information from code:

```c++
FILE *file = fopen("/proc/meminfo", "r");
char line[256];
unsigned long memTotal, memFree, memAvailable;

while (fgets(line, sizeof(line), file)) {
    if (strstr(line, "MemTotal:"))
        sscanf(line, "MemTotal: %lu kB", &memTotal);
    else if (strstr(line, "MemFree:"))
        sscanf(line, "MemFree: %lu kB", &memFree);
    else if (strstr(line, "MemAvailable:"))
        sscanf(line, "MemAvailable: %lu kB", &memAvailable);
}
fclose(file);
```

---

# Upgrading and Debugging

## ADB Debugging

### Connection Methods

- **Network connection (recommended)**: if the device supports Wi-Fi or Ethernet, open the corresponding settings screen to get the device's IP address, then choose **Debug configuration** -> **ADB configuration** in the IDE menu bar, enter the IP and save.
- **USB connection**: use a USB cable when the device has no network support; everything is fine if the computer recognizes it as an Android device. If you have driver problems, install the ADB driver.

> **Note**: if the device uses a type A (large) USB port, it is in mass storage mode by default; the following code switches it to ADB mode (takes effect after a reboot):
> ```c++
> #include "storage/StoragePreferences.h"
> StoragePreferences::putInt("sys_usb_mode_key", 1);
> ```

### Download and Debug

Once connected, right-click the project name in the project explorer and choose **Download and debug**, or use the shortcut `Ctrl+Alt+R`. The tool builds the project automatically and downloads the program to the device.

![](resources/ide/launch_adb.gif)

> **Note**: Download and debug does not flash the program permanently onto the device; the program is restored after removing the card or a power cycle. To flash it permanently, build an upgrade image.

### Flashing the Application via ADB

```bash
adb push ./update.img /tmp/update.img
adb shell setprop sys.zkupgrade.flag 255
adb shell setprop sys.zkupgrade.dir /tmp
adb shell setprop ctl.restart zkswe
```

### Common ADB Commands

After choosing **Debug configuration** -> **Open system command line** in the menu bar, you can run:

```bash
adb shell logcat -v time   # View the log with timestamps
adb shell logcat           # View the plain log
adb logcat -c              # Clear the log buffer
adb shell cat /proc/meminfo
adb shell busybox top
```

---

## Viewing Logs

### Adding Log Output

```c++
#include "utils/Log.h"

LOGD("onButtonClick_Button1\n");  // Used just like printf
```

### Viewing Logs from the Command Line (Recommended)

1. In the menu bar choose **Debug configuration** -> **Open system command line**
2. USB connection: enter `adb shell logcat -v time`
3. Network connection: first enter `adb connect 192.168.1.123`, then `adb shell logcat -v time`
4. `Ctrl+C` stops the log output

### Viewing the Standard Output Log

```bash
adb shell setprop ctl.stop zkswe   # Stop the app
zkgui                               # Start the app and watch its standard output
```

### Viewing the System Log

```bash
adb shell cat /proc/kmsg
```

---

## Booting the Program from a TF Card

> **Note: only FAT32-formatted TF cards are supported**

1. In the menu bar choose **Debug configuration** -> **ADB configuration**, click the `Settings` button and set the ADB download directory: `/mnt/extsd` (TF card) or `/mnt/usb1` (USB drive).
2. On the toolbar, click the dropdown arrow next to the debug button, choose **Path configuration**, select the TF card's drive letter and click OK.
3. Click the build button; the build output is packaged into the configured drive, producing `EasyUI.cfg`, `ui`, `lib`, `font` and other files.
4. Insert the TF card into the device and power it up again; once the system detects the files on the TF card it boots the program from the card.

---

## Building an Upgrade Image

This flashes the program permanently onto the device, so that it is not restored after a power cycle.

1. On the toolbar, click the dropdown arrow next to the image button, choose **Path configuration** and select the output directory for `update.img`.
2. Click the image build button to produce the `update.img` file.
3. Copy `update.img` to the root of a FAT32-formatted TF card, insert it into the device and power up again.
4. Once the system detects the upgrade file it shows the upgrade screen; tick the items to upgrade and click Upgrade.
5. Remove the TF card promptly after the upgrade finishes, to avoid upgrading repeatedly.

---

## Automatic Upgrade

If the screen is damaged or the touch input is inaccurate, you can create an extensionless file named `zkautoupgrade` in the root of the TF card; after inserting the card the system automatically ticks the upgrade items and starts the upgrade 2 seconds later.

- Putting a number in the `zkautoupgrade` file customizes the delay in seconds
- Putting a `zkrebootdelay` file with a number of seconds in the same directory controls the delay before rebooting after the upgrade completes (`-1` means do not reboot)

---

## Remote Upgrade

Download `update.img` to the device over HTTP, then trigger the upgrade check:

```c++
#include "os/UpgradeMonitor.h"

// Checks whether the given directory contains update.img and, if so, shows the upgrade dialog
UpgradeMonitor::getInstance()->checkUpgradeFile("/mnt/extsd/temp");
```

**Avoiding repeated upgrades**: download the image into a subdirectory such as `/mnt/extsd/temp/` (rather than `/mnt/extsd/`) and change the argument of the check function accordingly.

**Forcing an automatic update**: create the `zkautoupgrade` file after the download completes to trigger an automatic upgrade.

---

## Batch Upgrade

Upgrade several devices at once by IP address within the same local network.

1. Download and install the batch upgrade software.
2. Click `Settings` and configure:
   - The upgrade file (`update.img`)
   - The project name (matching the device's `ro.app.name` property)
   - The target upgrade version number (matching the device's `ro.app.version` property)
   - The IP configuration file (one IP per line, wildcards `*` supported)
3. After saving the settings, the tool connects to the devices and upgrades them automatically.

> **Note**: only devices connected over Wi-Fi or Ethernet are supported, not USB; only the Z20, Z21 and Z261 platforms are supported.

---

# Appendix

## FAQ

**Time-consuming work on the UI main thread prevents the upgrade screen from appearing after inserting the card**

Screen refreshing is handled on the UI main thread, so time-consuming work or an infinite loop on the main thread affects the UI refresh and prevents the upgrade screen from appearing after inserting the card. Callbacks such as `onUI_init` and `onUI_Timer` are all called on the UI main thread, so check whether any of them do time-consuming work. If this happens, power cycle the device; once the system comes up it checks for a pending upgrade first.

**Download and debug does not work / fails**

First check the connection type: devices with Wi-Fi only support a Wi-Fi connection, while devices without Wi-Fi can only be connected with a USB cable. Then choose **Debug configuration -> ADB configuration** in the menu bar, set the corresponding connection type and try downloading again.

**The program suddenly restarts by itself or freezes**

When something unexpected happens, look at the log first and try to reproduce the problem repeatedly. Crashes are mostly caused by incorrect pointer usage (null pointers, out-of-bounds array access, using memory after it has been freed, etc.). For freezes, look into **thread deadlocks**, **infinite loops in the code** and **long-running operations**.


---

## Installing the ADB Driver

For Windows XP / Vista / 7 / 8 / 10 / 11.

1. Download the [driver installer](resources/ADBDriverInstaller.zip)
2. Connect the device to the computer over USB.
3. Open ADB Driver Installer and click the **Install** button.
   - If the system warns that the driver publisher cannot be verified, choose **"Install this driver software anyway"**.
4. The driver is then installed automatically.

> Windows XP users are advised to reboot after installing the driver.
