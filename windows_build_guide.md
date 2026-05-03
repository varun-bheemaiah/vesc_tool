# ExiTool — Windows Build Guide

This guide walks you through building **ExiTool** from source on a fresh Windows 10 or Windows 11 install. No prior tooling is assumed.

Estimated time: 60–90 minutes (most of it Qt + MSVC downloads).

---

## 1. Overview of what you'll install

| Component | Purpose | Approx. download |
|-----------|---------|------------------|
| Visual Studio Build Tools 2022 (MSVC + Windows SDK) | C++ compiler + linker | ~3 GB |
| Qt 5.15.2 (MSVC 2019 64-bit) + add-on modules | UI framework | ~3 GB |
| Git for Windows | Clone the repo | ~70 MB |
| ExiTool source | This project | ~150 MB |

You can skip Git and download the repo as a ZIP if you prefer.

---

## 2. Install Visual Studio Build Tools 2022

ExiTool builds with the **MSVC 2019 64-bit** toolchain (which ships inside Visual Studio 2022).

1. Download the installer:
   <https://visualstudio.microsoft.com/visual-cpp-build-tools/>
   Click **Download Build Tools**.

2. Run `vs_BuildTools.exe`. When the **Workloads** screen opens:
   - Tick **Desktop development with C++**.

3. In the right-hand **Installation details** panel, make sure these are checked (they usually are by default):
   - `MSVC v143 - VS 2022 C++ x64/x86 build tools` (latest)
   - `Windows 11 SDK` (or Windows 10 SDK — either is fine)
   - `C++ CMake tools for Windows` (optional but useful)

4. Click **Install**. Wait until done. Reboot if prompted.

> If you already have the full **Visual Studio 2022 Community** edition installed with the C++ workload, you do not need Build Tools — Community includes everything Build Tools provides.

---

## 3. Install Qt 5.15.2

ExiTool targets Qt **5.15.2** (the LTS release of the 5.x line). Newer Qt 6 will **not** build this project without source changes.

### 3a. Create a Qt account (free)

The Qt online installer requires a free account.
Go to <https://login.qt.io/register> and register.

### 3b. Download the Qt Online Installer

<https://www.qt.io/download-qt-installer>
→ Download for Windows.

### 3c. Run the installer

1. Sign in with your Qt account.
2. On **Installation Folder**, leave the default `C:\Qt` (recommended — paths in this guide assume it).
3. On **Select Components**, expand **Qt → Qt 5.15.2** and check the following:

   **Required:**
   - [x] `MSVC 2019 64-bit`
   - [x] `Qt Quick Controls 2`
   - [x] `Qt Quick 3D` *(optional, only if you want 3D scenes)*
   - [x] `Qt Charts` *(optional)*
   - [x] `Qt Connectivity` (Bluetooth)
   - [x] `Qt Positioning`
   - [x] `Qt SerialPort`
   - [x] `Qt SVG`
   - [x] `Qt WebChannel` *(optional)*
   - [x] `Qt Gamepad` ← **important**, otherwise build fails
   - [x] `Sources` *(optional, for debugging into Qt)*

   **Developer and Designer Tools (default selection is fine):**
   - [x] `Qt Creator` (the IDE — use it if you don't want to build from the command line)
   - [x] `CMake`
   - [x] `Ninja`

4. Accept the LGPL/commercial terms and let it install. Qt 5.15.2 + tools is ~5 GB on disk.

> **If `Qt Gamepad` is missing in your installer:** Qt removed it from some installer manifests for 5.15.2. In that case skip it here and follow the **No qtgamepad** workaround in §7 below.

After install, you should have:
```
C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe
C:\Qt\Tools\QtCreator\bin\qtcreator.exe
```

---

## 4. Install Git for Windows

<https://git-scm.com/download/win>

Run the installer with default options. Open **Git Bash** or use the `git` command from any new Command Prompt.

> Skip this if you'll download the repo as a ZIP from GitHub.

---

## 5. Get the ExiTool source

Pick one:

**Option A — clone (preferred):**
```cmd
cd C:\
mkdir Projects
cd Projects
git clone https://github.com/varun-bheemaiah/vesc_tool.git
cd vesc_tool
```

**Option B — ZIP:**
1. GitHub → **Code → Download ZIP**.
2. Extract to e.g. `C:\Projects\vesc_tool`.

Throughout this guide the source root is `C:\Projects\vesc_tool`. Adjust paths if you put it elsewhere.

---

## 6. Build from the command line (recommended)

This is the simplest path. You'll use the **x64 Native Tools Command Prompt for VS 2022**, which sets up the MSVC environment automatically.

### 6a. Open the right command prompt

Start menu → search for **"x64 Native Tools Command Prompt for VS 2022"** → run.

You should see a banner like:
```
**********************************************************************
** Visual Studio 2022 Developer Command Prompt v17.x
** Copyright (c) 2022 Microsoft Corporation
**********************************************************************
[vcvarsall.bat] Environment initialized for: 'x64'
```

### 6b. Add Qt to PATH (this session only)

```cmd
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
```

Verify:
```cmd
qmake -v
```

Should print:
```
QMake version 3.1
Using Qt version 5.15.2 in C:\Qt\5.15.2\msvc2019_64\lib
```

### 6c. Configure and build

```cmd
cd C:\Projects\vesc_tool
qmake vesc_tool.pro CONFIG+=release CONFIG+=build_original CONFIG+=exclude_fw
nmake
```

Build flags explained:
- `CONFIG+=release` — optimized build, no debug symbols
- `CONFIG+=build_original` — selects the standard ExiTool variant (vs. the mobile/embedded variants)
- `CONFIG+=exclude_fw` — skips bundling the firmware blobs (saves space and time)

`nmake` runs single-threaded. To speed it up, use `jom` instead (ships with Qt Creator):
```cmd
C:\Qt\Tools\QtCreator\bin\jom\jom.exe
```
or just `jom` if you've added it to PATH.

The build takes 10–30 minutes depending on CPU.

### 6d. Locate the executable

After a successful build, the binary lives under:
```
C:\Projects\vesc_tool\build\win\ExiTool_x.xx.exe
```
(version number varies). It will **not** run yet — you still need to bundle the Qt DLLs.

### 6e. Bundle Qt DLLs with windeployqt

```cmd
mkdir dist\ExiTool
copy build\win\ExiTool_*.exe dist\ExiTool\
windeployqt --release --qmldir mobile --qmldir res\qml dist\ExiTool\ExiTool_*.exe
```

`windeployqt` copies every required Qt DLL (Core, Gui, Widgets, Quick, etc.), the platform plugin (`qwindows.dll`), and the QML modules referenced by ExiTool's `.qml` files. You can now zip `dist\ExiTool` and run it on any Windows machine without Qt installed.

Double-click `dist\ExiTool\ExiTool_*.exe` — the app should launch.

---

## 7. Workaround: build without `Qt Gamepad`

If you couldn't install the Qt Gamepad module (it has been intermittently missing from the 5.15.2 installer), you must comment out one line in the `.pro` file before running `qmake`.

Open `C:\Projects\vesc_tool\vesc_tool.pro` in a text editor and change:
```
DEFINES += HAS_GAMEPAD
```
to:
```
# DEFINES += HAS_GAMEPAD
```

Then run `qmake` and `nmake` as in §6c. Gamepad input will be unavailable in the resulting build, but everything else works.

> Don't commit this change — it's just for local builds when the module isn't installable.

---

## 8. Build inside Qt Creator (alternative)

If you'd rather use the IDE:

1. Open **Qt Creator**.
2. **File → Open File or Project…** → select `C:\Projects\vesc_tool\vesc_tool.pro`.
3. On the **Configure Project** screen, tick the kit **Desktop Qt 5.15.2 MSVC2019 64bit** and click **Configure Project**.
4. **Projects** tab (left sidebar) → under **Build Steps → qmake → Additional arguments**, add:
   ```
   CONFIG+=release CONFIG+=build_original CONFIG+=exclude_fw
   ```
5. Switch the build type at the bottom-left to **Release**.
6. Hit **Build → Run qmake**, then **Build → Build All** (`Ctrl+B`).
7. Run with `Ctrl+R`.

Qt Creator places its output in a sibling folder like `C:\Projects\build-vesc_tool-Desktop_Qt_5_15_2_MSVC2019_64bit-Release\`. To ship it, run `windeployqt` against the produced `.exe` exactly as in §6e.

---

## 9. Troubleshooting

### `'qmake' is not recognized`
You haven't added Qt to PATH. Run §6b again, or open a fresh command prompt and re-add it. Path is set per-shell; it doesn't persist unless you add it to your environment variables in System Properties.

### `'cl' is not recognized` or linker errors about missing CRT
You opened a regular Command Prompt instead of the **x64 Native Tools Command Prompt for VS 2022**. MSVC env vars aren't set. Reopen using the right shortcut.

### `Project ERROR: Unknown module(s) in QT: gamepad`
You don't have the Qt Gamepad module installed. Either install it via the Qt Maintenance Tool, or apply the workaround in §7.

### `LNK2019: unresolved external symbol` for Qt symbols
Mixed toolchain — Qt was installed for `msvc2019_64` but you're building with a different MSVC version, or a stale `Makefile` exists from a previous run. Run:
```cmd
nmake distclean
qmake vesc_tool.pro CONFIG+=release CONFIG+=build_original CONFIG+=exclude_fw
nmake
```

### Application starts but immediately closes / shows a `qwindows.dll` error
You forgot `windeployqt`, or you're running the raw `.exe` from `build\win\` instead of the deployed copy. Re-run §6e.

### Qt installer can't find a download mirror
Some corporate networks block the Qt CDN. Try a different network, or use the offline installer from <https://download.qt.io/official_releases/qt/5.15/5.15.2/>.

### `nmake: fatal error U1077: '...moc.exe' : return code '0xc0000135'`
Means a DLL it needs isn't on PATH. Make sure `C:\Qt\5.15.2\msvc2019_64\bin` is in PATH (§6b) — Qt's own tools (`moc`, `uic`, `rcc`) need their host Qt DLLs.

### Build is slow
Use `jom` (parallel `nmake` replacement, ships with Qt Creator):
```cmd
set PATH=C:\Qt\Tools\QtCreator\bin\jom;%PATH%
jom
```

---

## 10. Quick rebuild (after first successful build)

Whenever you pull new code or edit sources:

```cmd
cd C:\Projects\vesc_tool
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
nmake
```

Only re-run `qmake` if you change `vesc_tool.pro` or any `.ui` / `.qrc` file's structure. If the build ever behaves oddly:
```cmd
nmake distclean
qmake vesc_tool.pro CONFIG+=release CONFIG+=build_original CONFIG+=exclude_fw
nmake
```

---

## 11. Optional — Debug build

For step-debugging in Qt Creator:
```cmd
qmake vesc_tool.pro CONFIG+=debug CONFIG+=build_original CONFIG+=exclude_fw
nmake
```
The output ends up in `build\win\` with `d` appended to the file name (Qt convention for debug). Attach Qt Creator's debugger via **Debug → Start Debugging → Attach to Running Application**, or set up a **Custom Executable** run configuration pointing at the debug binary.

---

## 12. Done

You should have a working `ExiTool_*.exe` plus a deployable `dist\ExiTool\` folder. Plug in a controller via USB and the app will detect it — no driver install required on Windows 10/11 for VESC-class hardware.

If something in this guide is out of date, please open an issue or PR.
