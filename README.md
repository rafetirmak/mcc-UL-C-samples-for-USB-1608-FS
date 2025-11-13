# mcc-UL-C-samples-for-USB-1608-FS
Porting and refactoring the original Borland C sample applications for the MCC USB-1608FS DAQ device to compile cleanly with GCC. This repository documents the required code changes, provides working Linux-compatible builds, and offers a reference for developers integrating MCC devices into modern toolchains.

# MCC UL C Samples for USB-1608FS (GCC Port)

This repository provides **GCC-compatible ports** of the original **Measurement Computing (MCC) USB-1608FS** sample applications that were initially written in **Borland C**.

The aim is to:
- Modernize and clean up the legacy Borland C source code
- Make it compile cleanly with **GCC** (and optionally MinGW)
- Keep the **original behavior and logic** as intact as possible
- Serve as a reference for integrating MCC USB-1608FS devices into **modern toolchains** and **Linux/Unix environments**

> ⚠️ This project is *not* an official MCC product. It is an independent port of the publicly available sample codes.

---

## Contents

Suggested directory layout (you can adjust to match your actual structure):

- `original_borland/`  
  Original MCC sample sources as provided for Borland C (read-only, for reference).

- `src/`  
  Common, refactored source files shared by multiple samples (utility functions, wrappers, headers).

- `examples/`  
  GCC-compatible example programs for USB-1608FS:
  - `ai_single/` – single-channel/single-shot analog input example  
  - `ai_scan/` – multi-channel scan example  
  - `ao/` – analog output example  
  - `dio/` – digital I/O example  
  *(example list is illustrative; adapt to your real examples)*

- `docs/` (optional)  
  Additional notes about the porting process, differences between Borland C and GCC, and troubleshooting.

---

## Requirements

### Toolchain

- A C compiler:
  - `gcc` (Linux / Unix)
  - or `mingw-w64` (for Windows GCC builds, optional)
- `make` (or another build system; the repo assumes `make` by default)

### Libraries / Drivers

- MCC USB-1608FS drivers installed and working on your system
- MCC Universal Library **C** headers and libraries available to the compiler  
  (path configuration may be needed in `Makefile` or via environment variables)

> Check your MCC installation for the exact include and library paths and adjust the `Makefile` accordingly.

---

## Building

### Linux / Unix (GCC)

In the root of the repository:

```bash
# Build all examples
make

# Or build a specific example
make ai_single
make ai_scan
make ao
make dio
