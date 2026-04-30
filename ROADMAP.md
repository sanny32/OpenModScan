# OpenModScan 2 Roadmap

This document lists possible future features for OpenModScan version 2.x. It focuses on
features that complement the current application instead of duplicating the
existing Modbus TCP/RTU support, address scan, Modbus scanner, message parser,
user-defined messages, traffic log, statistics, simulation, printing, and
configuration profiles.

## OpenModScan 2 Project Workspace

The larger features in this roadmap are good candidates for OpenModScan 2. The
application can move from separate classic ModScan windows to a project-oriented
workspace with a project tree, tabbed documents, and shared log panels.

All project-related settings should be stored in a single `*.omscan` project
file. This file should include:

- connection settings;
- Modbus protocol settings;
- devices;
- register maps;
- display windows;
- scanner settings and scan results;
- data logger settings;
- trend views;
- watch and alarm rules;
- UI layout and opened tabs.

The current ini-based configuration can remain supported for compatibility, but
OpenModScan 2 should use `*.omscan` as the main project format.

## Priority 1: Register Maps

### Named register map

Add a device-oriented register map where each signal can have:

- name;
- Modbus point type;
- address;
- data type;
- byte/word order;
- scale and offset;
- unit;
- read/write access;
- optional minimum and maximum values;
- description or notes.

This would make OpenModScan more useful for real devices, where users usually
work with named signals rather than raw register addresses.

### Register map import and export

Support importing and exporting register maps using a simple format such as CSV
or JSON. This would allow users to share device templates, generate maps from
documentation, and keep register definitions under version control.

### Device templates

Build on top of register maps and allow users to save reusable device templates.
A template could include connection defaults, Modbus protocol settings, display
definition, and a named register map.

## Priority 2: Trending and Data Logging

### Live trends

Add live charts for selected registers or coils. Useful options:

- multiple signals on one chart;
- pause and resume;
- configurable time window;
- auto-scale and manual scale;
- export visible data.

This would help diagnose analog values, counters, setpoints, and slowly changing
process data.

### Structured data logger

Add a dedicated data logger for selected values. Unlike plain text capture, the
logger should produce structured records with:

- timestamp;
- device ID;
- point type;
- address;
- decoded value;
- raw value;
- status;
- response time.

CSV is the most practical first format. JSON Lines could be useful later for
automated processing.

### Snapshot comparison

Allow users to capture a snapshot of selected registers and compare it with a
later snapshot. The UI should highlight changed values, added/removed responses,
and failed reads.

This is useful when reverse engineering a device or checking what changed after
a configuration action.

## Priority 3: Diagnostics

### Watch and alarm rules

Allow users to define simple rules for monitored values:

- value is above or below a limit;
- value changed;
- value did not change for a configured time;
- bit is set or cleared;
- read failed.

Matching rules could highlight values in the UI and optionally write events to a
diagnostic log.

### Bitfield decoder

Add named bit definitions for holding and input registers. For example, a status
register could display individual flags such as Ready, Run, Fault, Local Mode,
or Remote Mode.

This would complement the existing bit pattern controls and make status words
easier to inspect.

### Diagnostic report export

Generate a compact report that includes:

- connection settings;
- protocol settings;
- scan results;
- selected register values;
- traffic/statistics summary;
- recent errors or exceptions.

Possible formats: PDF, HTML, or Markdown.

## Priority 4: Advanced Modbus Tools

### Ready-made diagnostic function dialogs

Expose additional Modbus functions as convenient dialogs where practical:

- 0x08 Diagnostics;
- 0x0B Get Comm Event Counter;
- 0x0C Get Comm Event Log;
- 0x11 Report Server ID.

The project already has broader Modbus message support, so these features could
reuse existing parsing and message infrastructure.

### Request sequence runner

Add a small sequence runner for repeatable test procedures:

- read one or more values;
- write a value;
- wait;
- check expected response;
- stop on error;
- export results.

This would help with commissioning, regression testing, and device acceptance
checks.

### Better error analysis

Improve diagnostics around failed requests:

- group repeated errors;
- show timeout and retry counters per window;
- show exception descriptions near the affected request;
- optionally suggest common causes, such as wrong address base, wrong point
  type, or unsupported function code.

## Possible Milestones

### Milestone 1

- Named register map model.
- CSV import/export for register maps.
- UI for selecting named signals in a ModScan window.

### Milestone 2

- Structured CSV data logger.
- Live trend window for selected values.
- Snapshot capture and comparison.

### Milestone 3

- Watch/alarm rules.
- Bitfield decoder.
- Diagnostic report export.

### Milestone 4

- Additional diagnostic function dialogs.
- Request sequence runner.
- Expanded device template workflow.
