# MDF4 Converters
This project includes library and converter tools for converting raw MDF4 log files.
These converters are designed for use with the [CANedge](https://www.csselectronics.com/screen/page/can-logger-products) CAN bus data loggers from [CSS Electronics](https://www.csselectronics.com/).

---

## Key features
```
1. Drag & drop log files to produce popular output formats
1. Drag & drop folders to easily batch convert files
2. Support for e.g. .csv, .asc (Vector), .trc (PEAK), .txt (CLX000) and more
3. Optionally use in any programming language for batch scripted processing
4. Optionally use via CLI for advanced options (e.g. folder conversion)
5. Open source - easily create your own converters or tweak existing ones
6. Windows/Linux support
```

---

## Library structure
The overall structure is as follows:

- The library that handles finalization and sorting is found in `Library`
- The generic tools utilising the library are found in `Tools`
  - `ASC` contains an exporter to ASCII format for e.g. Vector tools
  - `CLX000` contains an exporter to the raw log file format for CLX000 CAN loggers
  - `CSV` contains an exporter to comma separated files
  - `Finalizer` contains a tool for finalization & optional sorting of the MDF4 files
  - `PCAP` contains an exporter to the PCAP-NG format for import into Wireshark
  - `Shared` contains a base class library for all the other tools
  - `SocketCAN` contains an exporter to the log format from the SocketCAN utilities in Linux

---
## Converter tools
All the tools support drag-and-drop, where the default values are used to convert the data from the MDF4 file into
each tool's respective format. Additional options are present for all tools, which can be seen by running the tool in the
command line and passing either no arguments or `--help`.

For details on using the converters, see the CANedge Intros and the `README.md` in the `Tools/` folder.

---

## Building
The project uses CMake for configuration. All the project files are supposed to be built out-of-source, while the
main configuration will fetch and build the dependencies under the `External` folder.

If publishing any of the build artifacts, remember to change the "Company Name" in `Tools/CMakeLists.txt`, since the
information is embedded in the executable targets. 

For building under Windows using MinGW-64, Clang has to be configured as the compiler ([details](https://sourceforge.net/p/mingw-w64/bugs/727/))

---

## Quick start
Create a new folder for out-of-source building:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

This should notify that the external dependencies are missing. Build these using:
```
make Boost_builder
make botan_builder
make fmt_builder
make heatshrink_builder
make tinyxml2_builder
make appimage_builder (Linux only)
```

Refresh the project configuration now that the dependencies are present:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Build either all the targets, or the requested target.
```
make target
```

To see a list of currently available targets, CMake provides a help target, which list currently available targets.
```
make help
```

For convenience, targets for building all the tools are present:
- `ToolsRelease`: Build and pack the tools

In Windows, the latter option gathers all the executables in the `Release` folder under
the current build directory. Under Linux, this creates [AppImage](https://appimage.org/) packages (Still present in
each tool's folder). The build system must have a C++17 compiler, for instance Ubuntu 18.04 LTS. Build dependencies include python and pkg-config.

---

## Dependencies
The project uses the following external libraries, apart from the standard library for C++17:
- [AppImage packer](https://github.com/linuxdeploy/linuxdeploy) - MIT license
- [Boost](https://www.boost.org/) - Boost license
- [Botan](https://botan.randombit.net/) - Simplified BSD license
- [fmt](https://github.com/fmtlib/fmt) - Custom license, see repository for details
- [heatshrink](https://github.com/atomicobject/heatshrink) - ISC license
- [tinyxml2](https://github.com/leethomason/tinyxml2) - Zlib license

In the project the following header-only libraries are used and distributed:
- [mio](https://github.com/mandreyel/mio) - MIT license
- [SPIMPL](https://github.com/oliora/samples/blob/master/spimpl.h) - Boost license

### Boost license
Copyright Beman Dawes, Daniel Frey, David Abrahams, 2003-2004.
Copyright Rene Rivera 2004-2005.
Distributed under the [Boost Software License, Version 1.0](https://www.boost.org/LICENSE_1_0.txt).
