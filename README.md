# IPP Sharing

A lightweight tool to share local Windows printers driverlessly via IPP protocol. While ideal for home users, it may not suit enterprise environments due to the absence of advanced features like color management, custom paper sizes, user authentication, and audit logging.

This is a Windows-only tool. For Linux and macOS, CUPS is recommended for IPP-based printer sharing.

> **This fork** adds Windows 7 compatibility via proxy DLLs and binary patches. See [Windows 7 Compatibility](#windows-7-compatibility) below.

---

## Features

- **Lightweight**: Designed for simplicity and quick setup
- **Basic Print Ticket Support**: Standard media size, orientation, duplex, color mode
- **Apple AirPrint Compatible**: Seamless printing from Apple devices
- **Driver-Free Client Setup**: No drivers or PPD files needed on the client side
- **DNS-SD (Bonjour)**: Automatic printer service discovery

---

## Prerequisites

| Requirement | Notes |
|---|---|
| **Windows** | Win10+ (native), Win7 SP1 (via compatibility layer, see below) |
| **Rust** | [rustup](https://rustup.rs) with `x86_64-pc-windows-gnu` target |
| **MinGW-w64** | For compiling stub DLLs (`gcc` in PATH) |
| **Apple Bonjour** | For service discovery. Bundled with [iTunes](https://support.apple.com/en-us/HT210384) or [Bonjour Print Services](https://developer.apple.com/bonjour/) |

---

## Quick Start (Win10+)

### 1. Generate a self-signed certificate

```shell
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out certificate.pem -days 365 -nodes
```

### 2. Build

```shell
cargo build --release --target x86_64-pc-windows-gnu
```

### 3. Configure

Create `config.yaml` in the same directory as the binary:

```yaml
server:
  addr:
    - "[::]:631"
    - "0.0.0.0:631"
  tls:
    cert: "D:/ipp-sharing/certificate.pem"
    key: "D:/ipp-sharing/key.pem"
devices:
  - name: "Print To PDF (IppSharing)"
    info: "Print To PDF (IppSharing)"
    target: "Microsoft Print to PDF"
    uuid: "b27599fd-800c-409e-afe9-6dbbe11689ac"
    basepath: "/ipp/to_pdf"
    dnssd: true
```

> **Tip:** Generate a unique UUID for each printer at [uuidgenerator.net](https://www.uuidgenerator.net/).

### 4. Run

```shell
.\target\x86_64-pc-windows-gnu\release\ipp-sharing.exe
```

### 5. Firewall

Allow incoming connections on **TCP 631** (IPP) and **UDP 5353** (DNS-SD/Bonjour).

---

## Windows 7 Compatibility

The original project targets Windows 10. To run on **Windows 7 SP1**, this fork includes:

### The Problem

The `windows` crate (v0.62, pulled in via `winprint`) links against Win8+ APIs:

| Missing API | DLL | Win7 Status |
|---|---|---|
| `GetSystemTimePreciseAsFileTime` | kernel32.dll | ❌ Win8+ only |
| `GetHostNameW` | ws2_32.dll | ❌ Win8+ only |
| `WaitOnAddress` / `WakeByAddress*` | api-ms-win-core-synch-l1-2-0 | ❌ Win8+ only |
| `RoGetActivationFactory` | api-ms-win-core-winrt-l1-1-0 | ❌ Win8+ only |
| `RoOriginateErrorW` | api-ms-win-core-winrt-error-l1-1-0 | ❌ Win8+ only |
| `CoIncrementMTAUsage` | combase.dll | ❌ DLL doesn't exist on Win7 |
| `ProcessPrng` | bcryptprimitives.dll | ❌ Win8+ only |
| `PdfCreateRenderer` | windows.data.pdf.dll | ❌ WinRT PDF API |

Even though ipp-sharing never calls most of these in its actual code paths, the PE loader refuses to start if ANY imported DLL or function is missing.

### The Solution: Proxy DLLs + Binary Patching

**Step 1 — Binary patch:** Replace `GetSystemTimePreciseAsFileTime` with the Win2000-compatible `GetSystemTimeAsFileTime` directly in the compiled EXE.

**Step 2 — Proxy DLLs:** For each missing DLL, create a small stub DLL that either:
- Implements a Win7-compatible replacement (e.g., `ProcessPrng` → `SystemFunction036`)
- Forwards missing functions to the real system DLL
- Returns error codes for functions that are never actually called

All stub source code is in the `stubs/` directory.

### Build (Full, with Win7 Support)

```powershell
# 1. Build the Rust project
cargo build --release --target x86_64-pc-windows-gnu

# 2. Patch GetSystemTimePreciseAsFileTime → GetSystemTimeAsFileTime
python patch_ipp.py

# 3. Compile all proxy/stub DLLs
.\build_stubs.ps1
```

This produces `ipp-sharing_patched.exe` and 7 companion DLLs in `target\x86_64-pc-windows-gnu\release\`.

### Deploy to Windows 7

Copy these files to the target machine:

```
ipp-sharing_patched.exe
pdfium.dll
wsh_32.dll
api-ms-win-core-winrt-l1-1-0.dll
api-ms-win-core-winrt-error-l1-1-0.dll
api-ms-win-core-synch-l1-2-0.dll
bcryptprimitives.dll
combase.dll
windows.data.pdf.dll
```

Then on the **Win7 machine**, copy the system DLL:

```cmd
copy C:\Windows\System32\bcryptprimitives.dll bcryptprimitives_orig.dll
```

Also install **Visual C++ Redistributable 2015-2019 (x64)** from:
`https://aka.ms/vs/16/release/vc_redist.x64.exe`

### Win7 Prerequisites Summary

| Component | How to get |
|---|---|
| Win7 SP1 | Required baseline |
| vc_redist 2015-2019 x64 | `aka.ms/vs/16/release/vc_redist.x64.exe` |
| Bonjour | iTunes or Bonjour Print Services |

---

## Stub DLL Reference

| DLL | Source | What it does |
|---|---|---|
| `wsh_32.dll` | `ws2hook_mingw.c` + `.def` | Proxy for ws2_32: provides `GetHostNameW` via `gethostname` |
| `api-ms-win-core-winrt-l1-1-0.dll` | `stubs/winrt_stub.c` | Returns `E_NOTIMPL` for `RoGetActivationFactory` |
| `api-ms-win-core-winrt-error-l1-1-0.dll` | `stubs/winrt_error_stub.c` | Returns `TRUE` for `RoOriginateErrorW` |
| `api-ms-win-core-synch-l1-2-0.dll` | `stubs/synch_stub.c` + `.def` | Stubs `WaitOnAddress`/`WakeByAddress*`; forwards 50+ other functions to `kernel32.dll` |
| `bcryptprimitives.dll` | `stubs/bcrypt_stub.c` + `.def` | Replaces `ProcessPrng` with `SystemFunction036` (RtlGenRandom); forwards other functions to system copy (`bcryptprimitives_orig.dll`) |
| `combase.dll` | `stubs/combase_stub.c` + `.def` | Stubs `CoIncrementMTAUsage`; forwards `CoCreateFreeThreadedMarshaler` to `ole32.dll` |
| `windows.data.pdf.dll` | `stubs/winpdf_stub.c` | Returns `E_NOTIMPL` for `PdfCreateRenderer` |

---

## Adding a New Stub DLL

If the binary fails with "missing function X in Y.dll", add a new entry to `build_stubs.ps1`:

```powershell
# In build_stubs.ps1, add to $stubs array:
@{DLL="new.dll"; Src="stubs/new_stub.c"; Def=$null; Libs=""}
```

Then create `stubs/new_stub.c` using the existing stubs as templates.

---

## Configuration

```yaml
server:
  addr:
    - "[::]:631"
    - "0.0.0.0:631"
  tls:                    # Optional: TLS encryption
    cert: "certificate.pem"
    key: "key.pem"
devices:
  - name: "My Printer"
    info: "Office Printer"
    target: "Microsoft Print to PDF"
    uuid: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
    basepath: "/ipp/myprinter"
    dnssd: true
    make_and_model: "IppSharing via ippper"  # Optional
```

---

## License

    Copyright (C) 2024-2025 alampy.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

AGPL-3.0. See [LICENSE](LICENSE) for details.
