# Win32 ARM64 ISA version checks

This helper facility attempts to figure out ARM ISA level (i.e. ARMv8.2) it's currently running on.

**NOTE:** This is very early and unfinished concept. It needs a lot of improvement and feature tuning.

## Usage

To decide which `/arch:armvX.Y` -compiled executable should your launcher run.

While Windows API offers the
[IsProcessorFeaturePresent](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent)
API to check for various PF_ARM_XXX features, these only check presence of individual features.
That is good enough if you want to switch to hand-crafter intrinsics-using algorithm at runtime,
but there's not direct match to ISA feature level used by MSVC. This repository attempts to bridge that gap.

## Implementation

The helper parses undocumented/unsupported registry entries in `HARDWARE\\DESCRIPTION\\System\\CentralProcessor`, matches them against
[documented mandatory features](https://developer.arm.com/documentation/109697/2024_09/Feature-descriptions/The-Armv8-0-architecture-extension)
for those levels, optionally excluding features that are useless for user mode (applications), and returns determined ISA level.

## Assumptions

* Running on Windows on ARM
* ARMv8.0 is the baseline
* CRC32, AES and some other optional features of ARMv8.0 are always present, because they are mandatory for Windows on ARM
* Using Microsoft Visual Studio 2022 (17.11) or newer
