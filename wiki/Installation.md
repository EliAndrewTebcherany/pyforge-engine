# 📦 Installation Guide

This guide will walk you through setting up **pyforge-engine** on your system.

## Prerequisites

Before installing pyforge-engine, ensure you have the following installed:

### Required System Libraries

**On Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y python3-dev libglfw3-dev libglew-dev libopenal-dev libpython3-dev
```

**On macOS (using Homebrew):**
```bash
brew install glfw3 glew openal-soft python3
```

**On Windows (using vcpkg or pre-built binaries):**
- Install [Python 3.8+](https://www.python.org/downloads/)
- Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) with C++ support
- Download pre-built GLFW, GLEW, and OpenAL binaries or use package managers like `chocolatey` or `vcpkg`

### Required Python Version
- Python 3.8 or higher

## Installation Steps

### Step 1: Clone the Repository

```bash
git clone https://github.com/EliAndrewTebcherany/pyforge-engine.git
cd pyforge-engine
```

### Step 2: Create a Virtual Environment (Recommended)

Creating a virtual environment isolates your project dependencies:

```bash
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

### Step 3: Install Dependencies

The Makefile automates the installation process:

```bash
make
```

This command will:
- Upgrade pip and setuptools
- Install required Python packages (Pillow for image handling)
- Compile C extensions with OpenGL and OpenAL support

**Manual Installation Alternative:**
If you prefer to install manually without the Makefile:

```bash
pip install --upgrade pip setuptools
pip install Pillow
python setup.py build_ext --inplace
```

### Step 4: Verify Installation

Create a simple test script to verify the installation works:

```python
import pyforge

pyforge.init(800, 600, "PyForge Installation Test")
print("✓ PyForge Engine initialized successfully!")

while pyforge.is_open():
    pyforge.clear_gradient((0.1, 0.1, 0.1), (0.2, 0.2, 0.2))
    pyforge.refresh()
```

Run it with:
```bash
python test_installation.py
```

If a window opens successfully, your installation is complete!

---

## Troubleshooting

### Issue: "ModuleNotFoundError: No module named 'pyforge'"

**Solution:** Ensure you're in the virtual environment and have run `make` or `python setup.py build_ext --inplace`.

### Issue: "ImportError: cannot import name 'pyforge_core'"

**Solution:** The C extension failed to compile. Check that:
- Development headers are installed: `python3-dev` (Linux) or Xcode Command Line Tools (macOS)
- GLFW, GLEW, and OpenAL development libraries are installed
- Try compiling manually: `python setup.py build_ext --inplace`

### Issue: OpenGL/Graphics window won't open

**Solution:** 
- On Linux with remote sessions, ensure you have X11 or Wayland configured
- On WSL2, enable WSL GUI support
- Check that your GPU drivers are up to date

### Issue: "OSError: libopenal.so.1 not found"

**Solution:** Install OpenAL development libraries:
```bash
sudo apt-get install libopenal-dev  # Linux
brew install openal-soft            # macOS
```

### Issue: Build fails on Windows

**Solution:** 
- Ensure Visual Studio Build Tools C++ workload is installed
- Run the command prompt as Administrator
- Try installing pre-built wheels if available

---

## Next Steps

After successful installation, check out:
- **[Basic Game Tutorial](./Basic-Game-Tutorial)** — Learn to build your first game
- **[Function Reference Guide](../README.md#-global-function-reference-guide)** — API documentation

For additional help or issues, visit the [GitHub Issues](https://github.com/EliAndrewTebcherany/pyforge-engine/issues) page.
