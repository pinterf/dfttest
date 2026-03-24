# dfttest

2D/3D frequency domain denoiser for [Avisynth+](https://avs-plus.net/).

Original code by tritical, 16-bit modification by Firesledge, Avisynth+ port by DJATOM, high bit depth support and cross-platform work by pinterf.

## Requirements

**FFTW3 single-precision** library must be available at runtime (loaded dynamically):

- **Windows:** `libfftw3f-3.dll` must be in the DLL search path.
  Download from http://www.fftw.org/install/windows.html
- **Linux:** `libfftw3f.so.3`
  - Debian/Ubuntu: `sudo apt install libfftw3-3`
  - Fedora/RHEL: `sudo dnf install fftw-libs-single`

## Build

### Windows — Visual Studio

Open `dfttest.sln` (or `dfttest.slnx`) and build, or from the command line:

```
devenv dfttest.sln /Build "Release|x64"
```

Configurations: `Debug|Win32`, `Debug|x64`, `Release|Win32`, `Release|x64`, `ReleaseClangCL|x64`, `ReleaseXP|x64`

### Windows (MinGW/MSYS2) and Linux — CMake

```sh
cmake -B build -S .
cmake --build build -j$(nproc)
```

Force-disable Intel SIMD (e.g. for ARM — auto-detected otherwise):

```sh
cmake -B build -S . -DENABLE_INTEL_SIMD=OFF
```

Install (Linux):

```sh
cd build && sudo make install
```

Output: `build/dfttest/libdfttest.so` (Linux) or `build/dfttest/Release/dfttest.dll` (Windows)

## Syntax

```
dfttest(bool Y, bool U, bool V, int ftype, float sigma, float sigma2, float pmin,
        float pmax, int sbsize, int smode, int sosize, int tbsize, int tmode,
        int tosize, int swin, int twin, float sbeta, float tbeta, bool zmean,
        string sfile, string sfile2, string pminfile, string pmaxfile, float f0beta,
        string nfile, int threads, int opt, string nstring, string sstring,
        string ssx, string ssy, string sst, int dither, bool lsb, bool lsb_in,
        bool quiet)
```

## Parameters

**Y, U, V** (default: `true,true,true`)
If true, the corresponding plane is processed; otherwise copied through unchanged.

**ftype** (default: `0`)
Filter type:
- `0` — generalized Wiener filter: `mult = max((psd-sigma)/psd, 0)^f0beta`
- `1` — hard threshold: `mult = psd < sigma ? 0.0 : 1.0`
- `2` — multiplier: `mult = sigma`
- `3` — multiplier switched by psd: `mult = (psd >= pmin && psd <= pmax) ? sigma : sigma2`
- `4` — multiplier modified by psd range: `mult = sigma*sqrt((psd*pmax)/((psd+pmin)*(psd+pmax)))`

`psd` = magnitude squared = `real*real + imag*imag`

**sigma, sigma2** (default: `16.0, 16.0`)
Sigma values as described in `ftype`. Since v1.5, normalized based on the non-coherent power gain of the window (independent of window size and function). Use `sfile`/`sstring` to override per-coefficient.

**pmin, pmax** (default: `0.0, 500.0`)
Used as described in `ftype`. Since v1.5, normalized the same as sigma.

**sbsize** (default: `12`)
Spatial window side length. Must be ≥ 1. Must be odd if `smode=0`.

**smode** (default: `1`)
- `0` — process every pixel independently (no `sosize`)
- `1` — process in blocks of `sbsize`, with `sosize` overlap

**sosize** (default: `9`)
Spatial overlap. Range: `0` to `sbsize-1`. For overlap > 50%, `sbsize%(sbsize-sosize)` must equal 0.

**tbsize** (default: `5`)
Temporal window length (number of frames). Must be ≥ 1. Must be odd if `tmode=0`.

**tmode** (default: `0`)
- `0` — process every frame independently (no `tosize`)
- `1` — process in temporal blocks of `tbsize`, with `tosize` overlap

**tosize** (default: `0`)
Temporal overlap. Same constraints as `sosize`.

**swin, twin** (default: `0, 7`)
Analysis/synthesis window type for spatial (`swin`) and temporal (`twin`):
`0`=Hanning, `1`=Hamming, `2`=Blackman, `3`=4-term Blackman-Harris, `4`=Kaiser-Bessel,
`5`=7-term Blackman-Harris, `6`=Flat top, `7`=Rectangular, `8`=Bartlett,
`9`=Bartlett-Hann, `10`=Nuttall, `11`=Blackman-Nuttall

**sbeta, tbeta** (default: `2.5, 2.5`)
Beta value for Kaiser-Bessel window (`swin=4` / `twin=4`).

**zmean** (default: `true`)
If true, subtracts the window mean before frequency-domain filtering.

**sfile** (default: `""`)
Input file listing per-coefficient sigma values (one value per `tbsize*sbsize*((sbsize>>1)+1)` coefficients). Values separated by `,` or space; `#` comments a line.

**sfile2, pminfile, pmaxfile** (default: `"","",""`)
Same format as `sfile` for per-coefficient `sigma2`, `pmin`, and `pmax`.

**f0beta** (default: `1.0`)
Power exponent for `ftype=0`. `f0beta=1.0` → Wiener filter; `f0beta=0.5` → spectral subtraction. Values other than 1.0 and 0.5 use a slower general path with `pow()`.

**nfile** (default: `""`)
File listing noise-only block locations for automatic noise power spectrum estimation. Format per line: `frame,plane,ypos,xpos` (e.g. `0,0,20,20`). Add `a=value` on a line to set the over-subtraction factor (default: 5 for ftype=0, 7 for ftype=1). Output written to `noise_spectrum-<date>.txt`.

**threads** (default: `0`)
Number of processing threads. `0` = auto-detect (number of logical CPUs).

**opt** (default: `0`)
CPU optimization level:
- `0` — auto-detect
- `1` — C only
- `2` — SSE/SSE2
- `3` — SSE/SSE2/AVX
- `4` — SSE/SSE2/AVX/AVX2

**nstring** (default: `""`)
Same as `nfile` but inline. Quadruples separated by space; over-subtraction factor `a:value` must come first. Example: `nstring="a:4.0 35,0,45,68 28,0,23,87"`

**sstring** (default: `""`)
Sigma as a function of normalized frequency `[0.0, 1.0]`. Pairs of `freq:sigma` separated by space; must include endpoints 0.0 and 1.0. Values between pairs are linearly interpolated. Prefix with `$` to use fft3dfilter's distance formula `sqrt((x²+y²+z²)/3)` instead of per-dimension multiply. Example: `sstring="0.0:1.0 1.0:10.0"`. Output written to `filter_spectrum-<date>.txt`.

**ssx, ssy, sst** (default: `""`)
Per-dimension sigma functions (horizontal, vertical, temporal). Same syntax as `sstring`. If `sstring` is set, it takes precedence.

**dither** (default: `0`)
Output dithering when converting float → uint8:
- `0` — no dithering
- `1` — Floyd-Steinberg
- `2–100` — Floyd-Steinberg with increasing uniform random noise

**lsb** (default: `false`)
Output 16-bit stacked format (MSB top half, LSB bottom half; output height doubled).

**lsb_in** (default: `false`)
Input is 16-bit stacked format (same layout as `lsb=true` output). Sigma scale remains relative to MSB.

**quiet** (default: `true`)
Suppress filter spectrum file output when using `sstring`/`ssx`/`ssy`/`sst`.

## Changelog

**2026.03.24 v1.9.8**
- Cross-platform: builds on Linux (GCC/Clang) and Windows (MSVC/ClangCL/MinGW). CMakeLists.txt added. Intel SIMD auto-detected; disabled automatically on ARM/RISC-V.
- Threading: replaced Windows API (HANDLE, events, `_beginthreadex`, `CRITICAL_SECTION`) with C++17 `std::thread`, `std::mutex`, `std::condition_variable`.
- Thread safety: use Avisynth+ V12 global lock (`AcquireGlobalLock`/`ReleaseGlobalLock`) around FFTW plan creation and destruction. Falls back to `std::mutex` on older Avisynth+ versions.
- CPU detection: removed internal `CPUCheckForExtensions()`, use `env->GetCPUFlags()` instead.
- Dynamic library loading (FFTW3): platform-agnostic via `LoadLibrary`/`dlopen` abstraction.
- Updated Avisynth headers to V12 interface.

**2021.10.28 v1.9.7**
- Pass Avisynth+ frame properties.

**2020.03.24 v1.9.6**
- AVX2 routines for mod-8 sbsize cases; `opt=4` for AVX2.
- Fix: broken `filter_3_sse` (ftype=3).
- Fix: `filter_6_sse`: floor at `1e-15` instead of 0; replaced rsqrt+rcp with direct sqrt.

**2020.03.23 v1.9.5**
- High bit depth support: 10–32 bits, planar RGB and Y (greyscale).
- Fix: minor image noise at stacked16 input (`lsb_in=true`) — regression since 1.9.4.
- YUY2 converters replaced with SIMD intrinsics (no more external asm).

**2020.03.23 v1.9.4.4**
- Fix: FFTW plan thread safety.
- MSVC project updated to VS2019 (v142, v141_xp, ClangCL configs).
- Source to C++17 strict conformance; AVX always available.

**2018.10.14 v1.9.4.3 — DJATOM**
- Fixed crash on non-AVX CPUs.

**2017.09.04 — DJATOM**
- Adaptive MT mode: `MT_MULTI_INSTANCE` for `threads=1`, `MT_SERIALIZED` for >1.

**2017.08.14 — DJATOM**
- x64 port: inline asm replaced with intrinsics. AddMean/RemoveMean AVX paths.
- proc0_16 SSE2 path; PlanarFrame updated from NNEDI3 repo.

**2013-08-04 v1.9.4**
- Compatible with Avisynth 2.6 colorspaces (except Y8).

**2012-04-20 v1.9.3**
- No tbsize-related error on null-length clips.

**2012-03-23 v1.9.2**
- `quiet` parameter defaults changed.

**2012-03-11 v1.9.1**
- Fixed dither regression from v1.8 mod16a.

**2011-11-28 v1.9**
- Added `quiet` parameter.

**2011-05-12 v1.8 mod16b**
- Added `lsb_in` parameter.

**2010-06-26 v1.8 mod16a**
- Added `lsb` parameter.

**2010-06-22 v1.8**
- Added `dither` parameter; date strings on output files; `sstring` fft3dfilter mode.

**2010-06-21 v1.7**
- Added `nstring`/`sstring`/`ssx`/`ssy`/`sst`.

**2009-06-04 v1.6**
- Fixed window normalization for `tmode=0` and `smode=0`; `twin` default changed to 7.

**2009-04-11 v1.5**
- Added `f0beta`, `nfile`; sigma/pmin/pmax normalized by non-coherent power gain.

**2009-04-06 v1.4**
- Fixed threading issue causing corrupted output.

**2009-01-27 v1.3**
- More optimizations; `tmode=1` caching; temporal replication instead of mirroring.

**2009-01-24 v1.2**
- Added filter types 3/4 and parameters `sigma2`, `pmin`, `pmax`, `sfile2`, `pminfile`, `pmaxfile`.

**2007-11-22 v1.1**
- More SSE optimizations; fixed bottom-of-frame processing bug.

**2007-11-21 v1.0**
- Initial release.
