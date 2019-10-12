# MDF4 Converters
This project includes library and converter tools for converting raw MDF4 log files.
These converters are designed for use with the [CANedge](https://www.csselectronics.com/screen/page/can-logger-products) CAN bus data loggers from [CSS Electronics](https://www.csselectronics.com/).

---

### Key features
```
1. Drag & drop log file(s) to produce popular output formats
2. Support for e.g. `ASC`, `CSV`, `PCAP`, `SocketCAN` and more
3. Optionally use in any programming language for batch scripted processing
4. Optionally use via CLI for advanced options (e.g. folder conversion)
5. Open source - easily create your own converters or tweak existing ones
6. Windows/Linux support
```

---
### Structure
The overall structure is as follows:

- The library that handles finalization and sorting is found in `Library`
- The generic tools utilising the library are found in `Tools`
  - `ASC` contains an exporter to ASCII format for e.g. Vector tools
  - `CLX000` contains an exporter to the raw log file format for CLX000 CAN loggers
  - `CSV` contains an exporter to comma separated files
  - `PCAP` contains an exporter to the PCAP-NG format for import into Wireshark
  - `Shared` contains a base class library for all the other tools
  - `SocketCAN` contains an exporter to the log format from the SocketCAN utilities in Linux
  - `Tool` contains a tool for finalization & optional sorting of the MDF4 files 

---
### Tools
All the tools support drag-and-drop, where the default values are used to convert the data from the MDF4 file into
each tool's respective format. Additional options are present for all tools, which can be seen by running the tool in the
command line and passing either no arguments or `--help`.

---

### Building
The project uses CMake for configuration. All the project files are supposed to be built out-of-source, while the
main configuration will fetch and build the dependencies under the `External` folder.

If publishing any of the build artifacts, remember to change the "Company Name" in `Tools/CMakeLists.txt`, since the
information is embedded in the executable targets. 

---

### Quick start
Create a new folder for out-of-source building:
```
mkdir build
cd build
cmake ..
```

This should notify that the external dependencies are missing. Build these using:
```
make boost_builder
make date_builder
make tinyxml2_builder
```

Refresh the project configuration now that the dependencies are present:
```
cmake ..
```

Build either all the targets, or the requested target.
```
cmake DCMAKE_BUILD_TYPE=Release target
```

For convenience, targets for building all the tools are present:
- `Tools`: Only build the tools
- `Tools_pack`: Build and pack the tools.  

In Windows, the latter option gathers all the executables in the `Release` folder under
the current build directory. Under Linux, this creates [AppImage](https://appimage.org/) packages (Still present in
each tool's folder). The build system must have a C++17 compiler, for instance Ubuntu 18.04 LTS.

---

### Dependencies
The project uses the following external libraries, apart from the standard library for C++17:
- [Boost](https://www.boost.org/)  
- [tinyxml2](https://github.com/leethomason/tinyxml2)  
- [date](https://github.com/HowardHinnant/date)  

#### Boost license
Copyright Beman Dawes, Daniel Frey, David Abrahams, 2003-2004.
Copyright Rene Rivera 2004-2005.
Distributed under the [Boost Software License, Version 1.0](https://www.boost.org/LICENSE_1_0.txt).
