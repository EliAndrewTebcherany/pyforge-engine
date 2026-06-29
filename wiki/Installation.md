# 📦 Installation Guide

This guide will walk you through setting up **pyforge-engine** on your system.

## Standard Installation (Recommended)

Thanks to hardware-accelerated cross-platform binary wheels, you can install **pyforge-engine** instantly without needing a local C compiler (`gcc`), `make`, or setting up manual system dependency headers.

### Step 1: Create a Virtual Environment (Optional but Recommended)

Isolate your project dependencies by creating a virtual environment:

```bash
# On Linux / macOS
python3 -m venv venv
source venv/bin/activate

# On Windows
python -m venv venv
venv\Scripts\activate
```

### Step 2: Install via pip

Run a single command to automatically fetch the pre-compiled engine binaries along with all required dependencies (like Pillow):

```bash
pip install pyforge-engine
```

---

## Developer / Source Installation (Alternative)

If you are a contributor looking to modify the core C engine code (`src/core.c`, `src/text.c`, `src/effects.c`) or want to compile the library completely from scratch, follow these instructions.

### 1. Install Required System Libraries

You must have local C compilation tools and the development files for GLFW and OpenAL installed on your host operating system:

*   **Ubuntu/Debian:**
    ```bash
    sudo apt-get update
    sudo apt-get install -y python3-dev libglfw3-dev libopenal-dev build-essential
    ```
*   **macOS (using Homebrew):**
    ```bash
    brew install glfw openal-soft python3
    ```
*   **Windows:**
    *   Install [Python 3.10+](https://python.org)
    *   Install [Visual Studio Build Tools](https://microsoft.com) with the **Desktop development with C++** workload enabled.
    *   Use `vcpkg` to manage dependencies or manually configure pre-built Windows binaries for GLFW and OpenAL.

### 2. Clone and Local Compile

```bash
# Clone the repository
git clone https://github.com
cd pyforge-engine

# Build and install locally in editable developer mode using the Makefile
make
```

---

## Verify Installation

Create a simple script named `test_installation.py` to verify the engine initializes and hooks your GPU rendering pipelines correctly:

```python
import pyforge

# Initialize a hardware-accelerated window
pyforge.init(800, 600, "PyForge Installation Test")
print("✓ PyForge Engine initialized successfully!")

while pyforge.is_open():
    # Render loop using your custom GPU gradient clears
    pyforge.clear_gradient((0.1, 0.1, 0.1), (0.2, 0.2, 0.2))
    pyforge.refresh()
```

Run your test script:
```bash
python test_installation.py
```

If a clean, hardware-accelerated window pops open on your desktop, your setup is complete!

---

## Troubleshooting

### Issue: "ModuleNotFoundError: No module named 'pyforge'"
*   **Cause:** Your active terminal session is likely outside of the specific virtual environment where the library was installed.
*   **Solution:** Run `source venv/bin/activate` (or `venv\Scripts\activate` on Windows) before running your script.

### Issue: "ImportError: cannot import name 'pyforge_core'"
*   **Cause:** The pre-compiled wheel binary architecture didn't match your specific environment, or a manual source compilation phase failed.
*   **Solution:** Ensure your `pip` version is up to date by running `pip install --upgrade pip`. If you are developing from source code, confirm all system developer headers (like `libglfw3-dev`) are installed and run `make clean` before trying `make` again.

### Issue: OpenGL / Graphics window fails to open
*   **Cause:** Missing display drivers or running inside an environment lacking native display outputs (like standard headless servers).
*   **Solution:** 
    *   Ensure your local graphics card drivers are updated to the latest version.
    *   If you are testing inside **WSL2 (Windows Subsystem for Linux)**, ensure you are utilizing WSLg (WSL 2 GUI support available on updated Windows builds).
