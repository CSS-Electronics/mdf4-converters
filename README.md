# MDF4 Converters [LEGACY]
This project includes library and converter tools for converting raw MDF4 log files.
These converters are designed for use with the [CANedge](https://www.csselectronics.com/screen/page/can-logger-products) CAN bus data loggers from [CSS Electronics](https://www.csselectronics.com/).

**IMPORTANT**: Going forward, you can find the latest converter releases in the [MF4 converter docs](https://canlogger.csselectronics.com/tools-docs/converters_mf4/converters/). The source code in this repository matches release `v1.4.0`, but does not reflect the latest updates.

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
Major dependencies are expected to be present on the system already. These include:

* Boost
* Botan
* fmt

If publishing any of the build artifacts, remember to change the "Company Name" in `Tools/CMakeLists.txt`, since the
information is embedded in the executable targets.

If not present, they can be installed using vcpkg. The build uses a local overlay in `External\vcpkg-overlay` and requires the following targets:

* boost-bimap
* boost-filesystem
* boost-iostreams
* boost-log
* boost-dll
* boost-program-options
* botan
* fmt
* heatshrink
* neargye-semver

All targets are build using static linking. Select the matching triplet for the target system:

* `x64-windows-static`
* `x86-windows-static`
* `x64-linux`

#### Install vcpkg

Checkout vcpkg commit xyz and bootstrap it, such that it exists as a sub-folder in the project root. 

### Run CMake

Create a new folder for out-of-source building:

```
mkdir build
cd build
```

Run CMake with the desired configuration and the correct vcpkg triplet.

```
cmake -DCMAKE_BUILD_TYPE=$BUILD-TYPE -DVCPKG_OVERLAY_PORTS=../External/vcpkg-overlay -DVCPKG_TARGET_TRIPLET=$SELECTED-TRIPLET -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake ..
```

`$BUILD-TYPE` world normally be `Release`, but any other normal CMake build type can be inserted. Example:

```
cmake -DVCPKG_TARGET_TRIPLET=x64-linux -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release ..
```

If all dependencies are found, `make help` should provide a list of targets.

### Building Python extension
If the Python extension is to be build, build the `PyCXX_fecth` target first.

```
make PyCXX_fetct
rm CMakeLists.txt
```

And then re-run the CMake command to use the added files. Next, the wheel can be build using:

```
make mdf_iter_wheel
```

The resulting wheel file will be placed in `/path/to/mdf4-converters/Python/Distribution_Iterator/dist`.

### Building converter tools
If building the converters, either build the individual converter, or all using the `ToolsRelease` target.

```
make ToolsRelease
```

---

## Dependencies
The project uses the following external libraries, apart from the standard library for C++20:
- [Boost](https://www.boost.org/) - Boost license
- [Botan](https://botan.randombit.net/) - Simplified BSD license
- [fmt](https://github.com/fmtlib/fmt) - Custom license, see repository for details
- [heatshrink](https://github.com/atomicobject/heatshrink) - ISC license

In the project the following header-only libraries are used and distributed:
- [SPIMPL](https://github.com/oliora/samples/blob/master/spimpl.h) - Boost license
- [semver](https://github.com/Neargye/semver) - MIT license

### Boost license
Copyright Beman Dawes, Daniel Frey, David Abrahams, 2003-2004.
Copyright Rene Rivera 2004-2005.
Distributed under the [Boost Software License, Version 1.0](https://www.boost.org/LICENSE_1_0.txt).
