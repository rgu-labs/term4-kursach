# Project Overview

Course work on **"Probabilistic Modeling of Processes and Systems"** — 13 programming assignments in C++23. The course paper (пояснительная записка) is authored in YAML and compiled to LaTeX → PDF via `kursach-autogen`.

---

## Directory Structure

```
├── tasks/                    # 13 C++23 assignments
│   ├── 01-alphabet-sampling/ #   CLI (TaskRunner)
│   ├── 02-epidemic-simulation/ # Qt6 Quick GUI
│   ├── 03-rumor-spreading/   #   (source missing, only YAML section exists)
│   ├── 04-random-walk-2d/    #   Qt6 Quick GUI
│   ├── 05-cluster-pattern/   #   CLI (TaskRunner)
│   ├── 06-step-function/     #   Qt6 Quick GUI
│   ├── 07-complex-walk/      #   Qt6 Quick GUI
│   ├── 08-barrel-explosion/  #   CLI (TaskRunner)
│   ├── 09-cryptanalysis/     #   CLI (TaskRunner)
│   ├── 10-coin-tossing/      #   CLI (TaskRunner)
│   ├── 11-discrete-rv/       #   GLFW + ImGui + ImPlot
│   ├── 12-buffon-needle/     #   Qt6 Quick GUI
│   └── 13-graph-statistics/  #   CLI (nlohmann/json + spdlog)
├── kursach/                  # Course paper YAML sources
│   ├── kursach.yaml          #   Root document
│   ├── style/style.yaml      #   LaTeX style config
│   ├── sections/             #   Section YAMLs (task01..13, intro, references)
│   └── assets/               #   Screenshots (task01.png, task02_*.png, etc.)
├── scripts/
│   ├── build_kursach.sh      #   Main orchestrator
│   ├── build_tasks.sh        #   Build all tasks via CMake
│   ├── build_kursach_autogen.sh # Clone & build kursach-autogen from GitHub
│   └── verify_dependencies.sh #   Check required tools/packages
├── bin/kursach-autogen       # Pre-built Rust binary (YAML→LaTeX→PDF)
├── docs/kursach-autogen.md   # Full kursach-autogen format documentation
└── target/                   # Build output (PDF + compiled binaries)
```

---

## Build Pipeline

Run everything:
```bash
./scripts/build_kursach.sh -o target/
```

Steps:
1. **verify_dependencies.sh** — checks compiler, cmake, Qt6, GLFW, Rust, LaTeX packages
2. **build_kursach_autogen.sh** — if `bin/kursach-autogen` missing, clones from GitHub and `cargo build --release`
3. **build_tasks.sh** — for each task dir with `CMakeLists.txt`:
   - `cmake -S <task> -B <task>/build_release -G Ninja -DCMAKE_BUILD_TYPE=Release`
   - `cmake --build <task>/build_release --parallel 14`
   - Copies binary (+ `qml/`, `*.json`, `*.txt`) to `target/tasks/<name>/`
4. **kursach-autogen** — reads `kursach/kursach.yaml`, generates `main.tex`, runs `xelatex` → PDF
5. Final PDF → `target/kursach.pdf`

---

## kursach-autogen (YAML → LaTeX → PDF)

A Rust CLI tool. See `docs/kursach-autogen.md` for full reference.

### Root YAML (`kursach/kursach.yaml`)

```yaml
version: "1"
meta:
  university: "..."
  faculty: "..."
  department: "..."
  chair: "..."
  subject: "..."
  doc_type: "Курсовая работа"
  title: "..."
  author: { name: "...", group: "..." }
  supervisor: { name: "...", title: "..." }
  year: 2026
  city: "Москва"
  # logo: "assets/logo.png"

style: !import "style/style.yaml"

document:
  - !import "sections/intro.yaml"
  - !import "sections/task01.yaml"
  # ...
  - !import "sections/references.yaml"
```

### Style YAML (`kursach/style/style.yaml`)

Configures margins, fonts (main/heading/caption/listing), figure/table/listing caption format, bibliography standard (GOST).

### Section YAML (`kursach/sections/task*.yaml`)

```yaml
id: unique_id
title: "Section Title"
numbered: true
body:
  - type: paragraph
    text: "..."
  - type: formula
    id: eq.name
    content: "LaTeX math"
  - type: figure
    id: fig.name
    path: "assets/image.png"
    caption: "Description"
    width: 0.9
  - type: listing
    id: lst.name
    language: C++
    caption: "..."
    content: |
      source code...
  - type: table
    id: tbl.name
    caption: "..."
    columns: ["Col1", "Col2"]
    rows:
      - ["val1", "val2"]
  - type: list
    ordered: false
    items: ["item1", "item2"]
  - type: note / warning
    text: "..."
  - type: page_break
  - type: raw_latex
    content: "\\LaTeX{}"
subsections:
  - id: sub_id
    title: "..."
    numbered: true
    body: [ ... ]
```

### Bibliography (`references.yaml`)

```yaml
entries:
  - id: ref.name
    type: book   # book | article | online | thesis
    authors: ["Фамилия И.О."]
    title: "..."
    publisher: "..."
    city: "..."
    year: 2024
    pages: 256
```

### Inline Markers (in `text` fields)

- `{{cite:id}}` — citation reference
- `{{ref:id}}` — cross-reference (to any `id` in document)
- `{{pageref:id}}` — page reference
- `{{url:https://...}}` — URL
- `{{bold:text}}`, `{{italic:text}}`, `{{code:text}}`

All IDs across sections, blocks, and bibliography entries must be unique.

### Images in `assets/`

Images referenced in section YAMLs (figures) are stored in `kursach/assets/`. Some may be placeholder copies of `kosygin.png` — they exist so that `kursach-autogen` can resolve the file paths without errors. Replace them with real screenshots as needed.

### ⚠️ Known issues

- **`algorithm` block type** — buggy, do not use. Use `listing` blocks for algorithm/pseudocode instead.

---

## Task Architecture

### CLI Tasks (01, 05, 08, 09, 10)

- `CMakeLists.txt` — simple `add_executable`, C++23, no extra libs
- Use `TaskRunner` template class (`src/task_runner.hpp`):
  ```cpp
  TaskRunner rng;
  auto results = rng.run(experiment_fn, simulations, show_progress=true);
  ```
- `experiment_fn` is a callable `(RNG&) -> Result`
- `tally(results)` returns `std::map<Result, size_t>`
- Parameters via command line args (`argv`)
- RNG: `std::mt19937` seeded from `std::random_device`

### Qt6 Quick GUI Tasks (02, 04, 06, 07, 12)

- `CMakeLists.txt` — `find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)`, `qt_standard_project_setup()`, `qt_add_executable()`
- QML files in `qml/` — copied to `target/qml/` post-build
- Shared QML components: `Theme.qml`, `AppButton.qml`, `ParamSlider.qml`, `ModeTab.qml`, `StatBadge.qml`, `StatDivider.qml`, `SectionLabel.qml`, `FilePickerDialog.qml`
- Task-specific QML views: `GraphView.qml`, `NeedleView.qml`, `ComplexPlaneChart.qml`, etc.

### GLFW + ImGui Task (11 - Discrete RV)

- Links: `glfw`, `imgui`, `implot`, `GL`, `dl`, `pthread`
- Uses `ImGuiApp` wrapper class
- Plots: PMF (polyline), CDF, distribution law

### JSON I/O Task (13 - Graph Statistics)

- Links: `spdlog::spdlog`
- Uses `nlohmann/json` (header-only)
- Reads `config.json`, writes output JSON
- Custom memory allocators: arena, persistent

### Build System (CMake + Ninja)

- C++23 required, no extensions
- Release build type
- Common compile flags: `-Wall -Wextra -Wpedantic` (GCC/Clang)

---

## Configuration Files (JSON)

Tasks 06, 07, 13 use JSON config files (placed alongside the binary at build time):
```json
{
  "param1": value,
  "param2": "string"
}
```

---

## Verification

```bash
# Check all dependencies
./scripts/verify_dependencies.sh

# Build just tasks
./scripts/build_tasks.sh -i tasks/ -o target/tasks/

# Generate PDF only (requires kursach-autogen in bin/)
cd kursach && ../bin/kursach-autogen kursach.yaml --output ../tmp/kursach_out

# Run a CLI task
./target/tasks/01-alphabet-sampling/alphabet-sampling 100 1000000

# Run a Qt task
./target/tasks/02-epidemic-simulation/epidemic-simulation
```
