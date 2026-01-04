# Portable Process Trace Sampler (C)

`traceproc` is a lightweight, command-line **process tracing and sampling tool written in C**.  
It periodically samples runtime behavior of a target process and produces **timestamped, trace-style logs** in **CSV** or **JSON Lines (JSONL)** format.

The project is intentionally designed with a **portable architecture**: a shared core for sampling, logging, and output formatting, and **platform-specific backends** for accessing process metrics. While the current implementation targets **macOS**, the codebase is structured to support **Linux and Windows** backends.

This project demonstrates a **debug and trace tooling mindset** commonly used in **embedded systems and multi-core SoC software development**.

---

## Motivation

Modern software systems—particularly multi-core SoCs and embedded platforms—rely heavily on **debug and trace tools** to understand runtime behavior, performance characteristics, and correctness issues.

`traceproc` explores the core ideas behind such tools:
- periodic runtime sampling
- timing-aware, structured trace logs
- low-level system introspection
- clean tool-oriented software design

Although it runs in user space, the concepts closely align with observability techniques used in embedded Linux and SoC debug environments.

---

## Features

- Attach to a running process using its PID
- Periodic sampling with configurable interval
- Optional fixed-duration tracing
- Timestamped, structured trace output
- Output formats:
  - **CSV** (human-readable, spreadsheet-friendly)
  - **JSON Lines (JSONL)** (machine-readable, stream-friendly)
- Metrics captured (best-effort, platform-dependent):
  - process name
  - process state/status
  - resident memory (RSS)
  - virtual memory size
  - user CPU time
  - system CPU time
  - delta total CPU time between samples

---

## Architecture Overview

The project follows a clean separation of concerns:

### Core Logic
- CLI parsing
- Sampling loop
- Timestamping
- Delta calculation
- Output formatting

### Platform Backend
macOS (libproc)
Linux (planned)
Windows (planned)



This structure mirrors real-world engineering practices for building **portable tooling** across operating systems.


## Platform Support

| Platform | Status | Implementation |
|--------|--------|----------------|
| macOS  | Implemented | `libproc` (`proc_pidinfo`) |
| Linux  | Planned | `/proc/<pid>/stat`, `/proc/<pid>/status` |
| Windows | Planned | Win32 APIs (`OpenProcess`, `GetProcessTimes`, PSAPI) |

---

## Build Instructions

### macOS

Requirements:
- Xcode Command Line Tools (`clang`, system headers)
- CMake 3.16+

Build steps:

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```
The executable will be generated at:

```bash
./build/traceproc
```
## Usage
```bash
traceproc <pid>
  [--interval <ms>]
  [--duration <sec>]
  [--format csv|jsonl]
  [--out <file>]
  [--header]
```
## Options

| Option | Description |
|------|------------|
| `<pid>` | Process ID to trace |
| `--interval <ms>` | Sampling interval in milliseconds (default: 500) |
| `--duration <sec>` | Stop after N seconds (default: run until Ctrl+C or process exit) |
| `--format csv\|jsonl` | Output format |
| `--out <file>` | Write output to file instead of stdout |
| `--header` | Print CSV header line |


## Examples
### CSV output to stdout (with header)
```bash
./build/traceproc 1234 --interval 250 --format csv --header
```
JSONL output to file for 10 seconds
```bash
./build/traceproc 1234 --interval 500 --duration 10 --format jsonl --out trace.jsonl
```
Stop tracing at any time with Ctrl + C.

## Output Formats
### CSV
### Columns:

```
time, pid, name, state, rss_mib, vms_mib,
user_ms, sys_ms, total_ms, delta_total_ms
```
## JSON Lines (JSONL)
Each line is a standalone JSON object:
```
{
  "time": "2026-01-05T12:34:56.789",
  "pid": 1234,
  "name": "example",
  "state": "status=2",
  "rss_bytes": 10485760,
  "vms_bytes": 52428800,
  "user_ns": 123456789,
  "sys_ns": 98765432,
  "delta_total_ns": 4567890
}
```
## Implementation Notes (macOS)
- Process metrics are retrieved using libproc (proc_pidinfo with PROC_PIDTASKINFO)

- CPU time values are reported as best-effort nanoseconds based on OS-provided counters

- Process state information is provided on a best-effort basis via BSD process info

- Timing uses POSIX APIs (clock_gettime, nanosleep)

## Future Work
- Implement Linux backend using /proc filesystem

- Implement Windows backend using Win32 and PSAPI

- Add per-thread visibility where supported

- Introduce optional syscall or event-based tracing modes

- Add visualization scripts for CSV/JSONL output

